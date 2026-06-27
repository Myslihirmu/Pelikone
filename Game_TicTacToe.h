#ifndef GAME_TTT_H
#define GAME_TTT_H
#include "Globals.h"
#include "Comms.h"

int tttBoard[3][3];
int tttX = 0, tttY = 0, tttP = 1, tttW = 0;   // tttP: 1=X 2=O ; tttW: 0 none, 1 X, 2 O, 3 draw

// --- Phases / mode ---
#define TTT_MENU    0
#define TTT_PLAY    1
#define TTT_PAIRING 2
#define TTT_LOBBY   3
#define TTT_DISCONN 4
#define TTT_NET_STATE 1

int  tttPhase = TTT_MENU;
int  tttMode  = 0;          // 0 = 2 players (one device), 1 = network
int  tttMenuSel = 0;
int  tttMyPlayer = 1;       // network: 1 (X) or 2 (O)
bool tttOwnerIsX = true;
unsigned long tttPairStart = 0;

void checkTTTW() {
  for(int i=0; i<3; i++) {
    if(tttBoard[i][0]!=0 && tttBoard[i][0]==tttBoard[i][1] && tttBoard[i][1]==tttBoard[i][2]) tttW=tttBoard[i][0];
    if(tttBoard[0][i]!=0 && tttBoard[0][i]==tttBoard[1][i] && tttBoard[1][i]==tttBoard[2][i]) tttW=tttBoard[0][i];
  }
  if(tttBoard[0][0]!=0 && tttBoard[0][0]==tttBoard[1][1] && tttBoard[1][1]==tttBoard[2][2]) tttW=tttBoard[0][0];
  if(tttBoard[0][2]!=0 && tttBoard[0][2]==tttBoard[1][1] && tttBoard[1][1]==tttBoard[2][0]) tttW=tttBoard[0][2];
  if(tttW == 0) { bool full=true; for(int r=0; r<3; r++) for(int c=0; c<3; c++) if(tttBoard[r][c]==0) full=false; if(full) tttW=3; }
}

// --- Screens ---
void tttDrawMenu() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("RISTINOLLA");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);
  const char* opt[2] = { "2 PELAAJAA", "2 LAITETTA" };
  for (int i = 0; i < 2; i++) {
    int y = 58 + i * 26;
    if (i == tttMenuSel) { tft.fillRect(0, y - 15, 128, 20, 0x18C3); u8g2_display.setForegroundColor(COLOR_GREEN); }
    else u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, y);
    u8g2_display.print(i == tttMenuSel ? ">" : " ");
    u8g2_display.print(opt[i]);
  }
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 126); u8g2_display.print("ENTER = aloita");
}

void tttDrawPairing() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("RISTINOLLA");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(6, 58); u8g2_display.print("Etsitään toista");
  u8g2_display.setCursor(6, 78); u8g2_display.print("laitetta...");
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(6, 116); u8g2_display.print("ESC = peruuta");
}

void tttDrawLobby() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("RISTINOLLA");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);
  if (commsLobbyOwner) {
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, 44); u8g2_display.print("Kumpi olet?");
    const char* opt[2] = { "X", "O" };
    for (int i = 0; i < 2; i++) {
      int y = 70 + i * 22;
      if (i == tttMenuSel) { tft.fillRect(0, y - 15, 128, 20, 0x18C3); u8g2_display.setForegroundColor(COLOR_GREEN); }
      else u8g2_display.setForegroundColor(COLOR_TEXT);
      u8g2_display.setCursor(6, y);
      u8g2_display.print(i == tttMenuSel ? ">" : " ");
      u8g2_display.print(opt[i]);
    }
    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(2, 124); u8g2_display.print("X aloittaa");
  } else {
    u8g2_display.setForegroundColor(COLOR_GREEN);
    u8g2_display.setCursor(6, 56); u8g2_display.print("Yhdistetty!");
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, 84); u8g2_display.print("Toinen valitsee...");
  }
}

void tttDrawDisconn() {
  tft.fillRect(10, 40, 108, 48, COLOR_POPUP_BG);
  tft.drawRect(10, 40, 108, 48, COLOR_BORDER);
  u8g2_display.setForegroundColor(COLOR_RED);
  if (commsPartnerLeft) { u8g2_display.setCursor(20, 60); u8g2_display.print("Vastustaja"); u8g2_display.setCursor(20, 78); u8g2_display.print("poistui"); }
  else                  { u8g2_display.setCursor(20, 60); u8g2_display.print("Yhteys"); u8g2_display.setCursor(20, 78); u8g2_display.print("katkesi"); }
}

void drawTTTBoard() {
  if (tttPhase == TTT_MENU)    { tttDrawMenu();    return; }
  if (tttPhase == TTT_PAIRING) { tttDrawPairing(); return; }
  if (tttPhase == TTT_LOBBY)   { tttDrawLobby();   return; }

  tft.fillScreen(COLOR_BG);
  tft.drawFastVLine(42, 0, 128, COLOR_GRAY); tft.drawFastVLine(85, 0, 128, COLOR_GRAY);
  tft.drawFastHLine(0, 42, 128, COLOR_GRAY); tft.drawFastHLine(0, 85, 128, COLOR_GRAY);
  for(int r=0; r<3; r++) for(int c=0; c<3; c++) {
    int px = c * 43 + 12, py = r * 43 + 32;
    if(tttBoard[r][c] == 1) { u8g2_display.setForegroundColor(COLOR_GREEN); u8g2_display.setCursor(px, py); u8g2_display.print("X"); }
    else if(tttBoard[r][c] == 2) { u8g2_display.setForegroundColor(COLOR_YELLOW); u8g2_display.setCursor(px, py); u8g2_display.print("O"); }
  }
  tft.drawRect(tttX * 43 + 2, tttY * 43 + 2, 39, 39, COLOR_TEXT);

  if (tttPhase == TTT_DISCONN) { tttDrawDisconn(); return; }

  if(tttW > 0) {
    tft.fillRect(10, 40, 108, 48, COLOR_BG); u8g2_display.setForegroundColor(COLOR_TEXT); u8g2_display.setCursor(20, 65);
    u8g2_display.print(tttW == 3 ? "DRAW!" : (tttW == 1 ? "WINNER: X" : "WINNER: O"));
  }
}

// --- Network serialization (whole board each move) ---
void tttSendState() {
  uint8_t b[13];
  b[0] = TTT_NET_STATE;
  b[1] = tttOwnerIsX ? 1 : 0;
  int k = 2;
  for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++) b[k++] = (uint8_t)tttBoard[r][c];
  b[11] = (uint8_t)tttP;
  b[12] = (uint8_t)tttW;
  commsSend(b, 13);
}

void tttRecvState(const uint8_t* b) {
  tttOwnerIsX = (b[1] != 0);
  tttMyPlayer = commsLobbyOwner ? (tttOwnerIsX ? 1 : 2) : (tttOwnerIsX ? 2 : 1);
  int k = 2;
  for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++) tttBoard[r][c] = b[k++];
  tttP = b[11];
  tttW = b[12];
  if (tttPhase == TTT_LOBBY) { tttX = 0; tttY = 0; }
  tttPhase = TTT_PLAY;
  drawTTTBoard();
}

// --- Game flow ---
void setupTTT() {             // launched from main menu -> show the chooser
  tttPhase = TTT_MENU;
  tttMenuSel = 0;
  drawTTTBoard();
}

void tttStartGame() {         // 2 players on one device
  for(int r=0; r<3; r++) for(int c=0; c<3; c++) tttBoard[r][c]=0;
  tttW=0; tttP=1; tttX=0; tttY=0;
  tttPhase = TTT_PLAY;
  drawTTTBoard();
}

void tttStartNetGame() {      // owner starts/restarts a network game
  for(int r=0; r<3; r++) for(int c=0; c<3; c++) tttBoard[r][c]=0;
  tttW=0; tttP=1; tttX=0; tttY=0;
  tttPhase = TTT_PLAY;
  drawTTTBoard();
  tttSendState();
}

void tttOnExit() {            // called by the .ino when leaving via ESC
  if (tttMode == 1) { commsEnd(); tttMode = 0; tttPhase = TTT_MENU; }
}

void updateTTT() {            // called every loop; only does work in network mode
  if (tttMode != 1) return;
  commsTick();

  if ((commsPartnerLeft || commsLinkLost) && (tttPhase == TTT_LOBBY || tttPhase == TTT_PLAY)) {
    tttPhase = TTT_DISCONN;
    drawTTTBoard();
    return;
  }

  if (tttPhase == TTT_PAIRING) {
    if (commsPaired) { tttPhase = TTT_LOBBY; tttMenuSel = 0; drawTTTBoard(); }
    else if (millis() - tttPairStart > 30000) { tttOnExit(); setupTTT(); }
    return;
  }

  if (tttPhase == TTT_LOBBY || tttPhase == TTT_PLAY) {
    uint8_t buf[32]; uint8_t len;
    if (commsPoll(buf, &len) && len >= 13 && buf[0] == TTT_NET_STATE) tttRecvState(buf);
  }
}

void handleTTTInput(char c) {
  if (tttPhase == TTT_MENU) {
    if      (c == 0xB5) { if (tttMenuSel > 0) tttMenuSel--; drawTTTBoard(); }
    else if (c == 0xB6) { if (tttMenuSel < 1) tttMenuSel++; drawTTTBoard(); }
    else if (c == 0x0D || c == 0x0A) {
      if (tttMenuSel == 0) { tttMode = 0; tttStartGame(); }
      else { tttMode = 1; commsBegin(); tttPhase = TTT_PAIRING; tttPairStart = millis(); drawTTTBoard(); }
    }
    return;
  }

  if (tttPhase == TTT_PAIRING || tttPhase == TTT_DISCONN) return;

  if (tttPhase == TTT_LOBBY) {
    if (commsLobbyOwner) {
      if      (c == 0xB5) { tttMenuSel = 0; drawTTTBoard(); }
      else if (c == 0xB6) { tttMenuSel = 1; drawTTTBoard(); }
      else if (c == 0x0D || c == 0x0A) {
        tttOwnerIsX = (tttMenuSel == 0);
        tttMyPlayer = tttOwnerIsX ? 1 : 2;
        tttStartNetGame();
      }
    }
    return;
  }

  // TTT_PLAY
  if(tttW > 0) {
    if(c == 0x0D || c == 0x0A) {
      if (tttMode == 1) { if (commsLobbyOwner) tttStartNetGame(); }
      else tttStartGame();
    }
    return;
  }
  if(c == 0xB4) tttX = (tttX > 0) ? tttX - 1 : 2;
  else if(c == 0xB7) tttX = (tttX < 2) ? tttX + 1 : 0;
  else if(c == 0xB5) tttY = (tttY > 0) ? tttY - 1 : 2;
  else if(c == 0xB6) tttY = (tttY < 2) ? tttY + 1 : 0;
  else if(c == 0x0D || c == 0x0A) {
    if (tttMode == 1 && tttP != tttMyPlayer) { drawTTTBoard(); return; }   // not your turn
    if(tttBoard[tttY][tttX] == 0) {
      tttBoard[tttY][tttX] = tttP; tttP = (tttP == 1) ? 2 : 1; checkTTTW();
      if (tttMode == 1) tttSendState();
    }
  }
  drawTTTBoard();
}
#endif
