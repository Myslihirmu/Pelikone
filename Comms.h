#ifndef COMMS_H
#define COMMS_H
// Reusable ESP-NOW link for same-room two-player games.
//
//   commsBegin()  - radio on, start auto-pairing (broadcast HELLO)
//   commsTick()   - call every loop: drives pairing, resends, timeout
//   commsPaired   - true once two devices have found each other
//   commsLobbyOwner - true on ONE device (lower MAC); it runs the lobby
//   commsSend(buf,len) / commsPoll(buf,&len) - exchange game payloads
//   commsPartnerLeft - opponent pressed quit ; commsLinkLost - went silent
//   commsEnd()    - tell partner we left, radio off (saves battery)
//
// Reliability: the last payload is re-sent until the partner replies, and a
// brief drop-out recovers on its own as long as it's under COMMS_TIMEOUT_MS.
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <string.h>

#define COMMS_CHANNEL      1
#define COMMS_MAX_PAYLOAD  200
#define COMMS_HELLO_MS     400      // broadcast HELLO interval while pairing
#define COMMS_RESEND_MS    1500     // resend last payload until partner replies
#define COMMS_TIMEOUT_MS   15000    // no contact for this long -> link lost

#define COMMS_T_HELLO 1
#define COMMS_T_APP   2
#define COMMS_T_QUIT  3

static const uint8_t COMMS_BCAST[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// --- public state ---
volatile bool commsPaired      = false;
volatile bool commsPartnerLeft = false;
volatile bool commsLinkLost    = false;
bool          commsLobbyOwner  = false;

// --- internal state ---
static uint8_t commsOwnMac[6];
static uint8_t commsPartnerMac[6];
static volatile bool    commsHaveCandidate = false;
static uint8_t          commsCandidateMac[6];
static volatile bool    commsReplyHello = false;
static volatile unsigned long commsLastInbound = 0;

static portMUX_TYPE commsMux = portMUX_INITIALIZER_UNLOCKED;
static volatile bool    commsInNew = false;
static uint8_t          commsInBuf[COMMS_MAX_PAYLOAD];
static volatile uint8_t commsInLen = 0;

static uint8_t       commsTxBuf[COMMS_MAX_PAYLOAD];
static uint8_t       commsTxLen = 0;
static bool          commsTxPending = false;
static unsigned long commsLastTx = 0;
static unsigned long commsLastHello = 0;

// --- helpers ---
static void commsSendRaw(const uint8_t* mac, uint8_t type, const uint8_t* data, uint8_t len) {
  uint8_t frame[1 + COMMS_MAX_PAYLOAD];
  frame[0] = type;
  if (data && len) memcpy(frame + 1, data, len);
  esp_now_send(mac, frame, 1 + len);
}

static void commsAddPeer(const uint8_t* mac) {
  if (esp_now_is_peer_exist(mac)) return;
  esp_now_peer_info_t p;
  memset(&p, 0, sizeof(p));
  memcpy(p.peer_addr, mac, 6);
  p.channel = COMMS_CHANNEL;
  p.encrypt = false;
  esp_now_add_peer(&p);
}

// Receive callback — runs in the WiFi task, so keep it short.
static void commsOnRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  if (len < 1) return;
  const uint8_t* mac = info->src_addr;
  if (memcmp(mac, commsOwnMac, 6) == 0) return;
  uint8_t type = data[0];
  commsLastInbound = millis();
  commsLinkLost = false;

  if (type == COMMS_T_HELLO) {
    if (!commsPaired) {
      if (!commsHaveCandidate) { memcpy(commsCandidateMac, mac, 6); commsHaveCandidate = true; }
    } else if (memcmp(mac, commsPartnerMac, 6) == 0) {
      commsReplyHello = true;   // partner still pairing -> answer from commsTick()
    }
    return;
  }

  if (!commsPaired || memcmp(mac, commsPartnerMac, 6) != 0) return;

  if (type == COMMS_T_QUIT) { commsPartnerLeft = true; return; }

  if (type == COMMS_T_APP) {
    portENTER_CRITICAL(&commsMux);
    commsInLen = (len - 1 > COMMS_MAX_PAYLOAD) ? COMMS_MAX_PAYLOAD : (uint8_t)(len - 1);
    memcpy(commsInBuf, data + 1, commsInLen);
    commsInNew = true;
    portEXIT_CRITICAL(&commsMux);
    commsTxPending = false;     // partner replied -> stop resending
  }
}

// --- public API ---
void commsBegin() {
  commsPaired = false; commsPartnerLeft = false; commsLinkLost = false; commsLobbyOwner = false;
  commsHaveCandidate = false; commsReplyHello = false; commsInNew = false;
  commsTxPending = false; commsTxLen = 0; commsLastHello = 0;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(COMMS_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_get_mac(WIFI_IF_STA, commsOwnMac);

  esp_now_init();
  esp_now_register_recv_cb(commsOnRecv);
  commsAddPeer(COMMS_BCAST);

  commsLastInbound = millis();
}

void commsEnd() {
  if (commsPaired) commsSendRaw(commsPartnerMac, COMMS_T_QUIT, NULL, 0);
  esp_now_deinit();
  WiFi.mode(WIFI_OFF);
  commsPaired = false;
}

void commsSend(const uint8_t* data, uint8_t len) {
  if (!commsPaired) return;
  if (len > COMMS_MAX_PAYLOAD) len = COMMS_MAX_PAYLOAD;
  memcpy(commsTxBuf, data, len);
  commsTxLen = len;
  commsTxPending = true;
  commsLastTx = millis();
  commsSendRaw(commsPartnerMac, COMMS_T_APP, data, len);
}

// Returns true and copies the latest payload if one arrived since last poll.
bool commsPoll(uint8_t* buf, uint8_t* len) {
  bool got = false;
  portENTER_CRITICAL(&commsMux);
  if (commsInNew) {
    memcpy(buf, commsInBuf, commsInLen);
    *len = commsInLen;
    commsInNew = false;
    got = true;
  }
  portEXIT_CRITICAL(&commsMux);
  return got;
}

void commsTick() {
  unsigned long now = millis();

  // Finalize pairing here (loop context), not inside the callback.
  if (commsHaveCandidate && !commsPaired) {
    memcpy(commsPartnerMac, commsCandidateMac, 6);
    commsAddPeer(commsPartnerMac);
    commsPaired = true;
    commsLobbyOwner = (memcmp(commsOwnMac, commsPartnerMac, 6) < 0);
    commsLastInbound = now;
    commsSendRaw(commsPartnerMac, COMMS_T_HELLO, NULL, 0);   // help partner pair too
  }

  if (!commsPaired) {
    if (now - commsLastHello >= COMMS_HELLO_MS) {
      commsSendRaw(COMMS_BCAST, COMMS_T_HELLO, NULL, 0);
      commsLastHello = now;
    }
    return;
  }

  if (commsReplyHello) {
    commsReplyHello = false;
    commsSendRaw(commsPartnerMac, COMMS_T_HELLO, NULL, 0);
  }

  if (commsTxPending && now - commsLastTx >= COMMS_RESEND_MS) {
    commsSendRaw(commsPartnerMac, COMMS_T_APP, commsTxBuf, commsTxLen);
    commsLastTx = now;
  }

  if (now - commsLastInbound >= COMMS_TIMEOUT_MS) commsLinkLost = true;
}

#endif
