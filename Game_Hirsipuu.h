#ifndef GAME_HIRSIPUU_H
#define GAME_HIRSIPUU_H
#include "Globals.h"

// --- Word list (lengths 4-9, includes Ä/Ö) ---
const char* const hp_words[] PROGMEM = {
  // 4-letter (17)
  "AUTO","TALO","MERI","PELI","MATO","KALA","LUMI","PALO","KATU","LELU",
  "ILMA","JOKI","KOTI","OKSA","KIVI",
  "KESÄ","ÖLJY",
  // 5-letter (26)
  "KOIRA","KISSA","LINTU","LEHTI","KUKKA","PALLO","KIRJA","PERHE","LAPSI",
  "KARHU","KETTU","HIRVI","KOULU","TALVI","SYKSY","KAHVI","LAULU","PAITA",
  "HATTU","KELLO",
  "TÄHTI","METSÄ","JÄRVI","KEVÄT","PÖYTÄ","HÄRKÄ","SÄNKY","PÖLLÖ",
  // 6-letter (17)
  "KIRKKO","KAUPPA","KALLIO","LAMMAS","KANGAS","KORTTI","NUMERO","MUISTO",
  "PERUNA","KUPARI","LOISTO","LAUKKU","SAAPAS","TONTTU","TALTTA",
  "KÄÄRME","TÄYNNÄ",
  // 7-letter (10)
  "MAKKARA","HARAKKA","POLIISI","LUSIKKA","TULOKAS","PALVELU","KORVAUS",
  "RATIKKA","SINAPPI","PAPRIKA",
  // 8-letter (7)
  "MANSIKKA","TOMAATTI","KAUPUNKI","KIRJASTO","SAIRAALA","TAKAISIN","PESUKONE",
  // 9-letter (3)
  "TIETOKONE","YLIOPISTO","TELEVISIO"
};
const int HP_WORD_COUNT = 80;
const int HP_MAX_WRONG  = 9;

// --- Game state ---
enum HPPhase { HP_SUBMENU, HP_INPUT, HP_PASS, HP_PLAYING, HP_WIN, HP_LOSE };
HPPhase hpPhase   = HP_SUBMENU;
int     hpSel     = 0;
String  hpWord    = "";
String  hpInput   = "";
String  hpGuessed = "";   // concatenated UTF-8 letter strings
int     hpWrong   = 0;

// --- UTF-8 helpers (same logic as Sanuli's getLetter) ---

// Number of logical characters (Ä/Ö count as 1 each)
int hpWordLen(const String& s) {
  int count = 0, i = 0;
  while (i < (int)s.length()) {
    i += (((uint8_t)s[i] & 0xE0) == 0xC0) ? 2 : 1;
    count++;
  }
  return count;
}

// Get logical character at index as a String (1 or 2 bytes)
String hpGetLetter(const String& s, int idx) {
  int logical = 0, i = 0;
  while (i < (int)s.length()) {
    int len = (((uint8_t)s[i] & 0xE0) == 0xC0) ? 2 : 1;
    if (logical == idx) return s.substring(i, i + len);
    i += len; logical++;
  }
  return "";
}

// Is this letter (String) already in hpGuessed?
bool hpIsGuessed(const String& letter) {
  return hpGuessed.indexOf(letter) >= 0;
}

// --- Draw gallows + body parts ---
// Stage 0: ground only
// Stage 1: + post
// Stage 2: + beam (90° turn)
// Stage 3: + rope
// Stage 4: + head
// Stage 5: + torso
// Stage 6: + left arm
// Stage 7: + right arm
// Stage 8: + left leg
// Stage 9: + right leg (lose)
void hpDrawGallows(int wrong) {
  uint16_t col = COLOR_TEXT;
  tft.drawFastHLine(4,  78, 28, col);                              // 0: ground
  if (wrong >= 1) tft.drawFastVLine(18, 22, 56, col);             // 1: post
  if (wrong >= 2) tft.drawFastHLine(18, 22, 36, col);             // 2: beam
  if (wrong >= 3) tft.drawFastVLine(54, 22, 10, col);             // 3: rope
  if (wrong >= 4) tft.drawCircle(54, 41, 8, col);                 // 4: head
  if (wrong >= 5) tft.drawFastVLine(54, 49, 18, col);             // 5: torso
  if (wrong >= 6) tft.drawLine(54, 55, 44, 65, col);              // 6: left arm
  if (wrong >= 7) tft.drawLine(54, 55, 64, 65, col);              // 7: right arm
  if (wrong >= 8) tft.drawLine(54, 67, 44, 78, col);              // 8: left leg
  if (wrong >= 9) tft.drawLine(54, 67, 64, 78, col);              // 9: right leg
}

// --- Main draw function ---
void drawHirsipuuBoard() {
  tft.fillScreen(COLOR_BG);

  // ── Submenu ──────────────────────────────────────────────────────────────
  if (hpPhase == HP_SUBMENU) {
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(2, 15);
    u8g2_display.print("HIRSIPUU");
    tft.drawFastHLine(0, 19, 128, COLOR_GRAY);

    const char* labels[2] = { "Satunnainen", "Syota sana" };
    for (int i = 0; i < 2; i++) {
      if (i == hpSel) {
        tft.fillRect(0, 26 + i * 26, 128, 24, 0x18C3);
        u8g2_display.setForegroundColor(COLOR_GREEN);
      } else {
        u8g2_display.setForegroundColor(COLOR_TEXT);
      }
      u8g2_display.setCursor(8, 42 + i * 26);
      u8g2_display.print(i == hpSel ? ">" : " ");
      u8g2_display.print(labels[i]);
    }
    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(4, 115);
    u8g2_display.print("ENTER = valitse");
    return;
  }

  // ── Player 1 word input ──────────────────────────────────────────────────
  if (hpPhase == HP_INPUT) {
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(2, 15);
    u8g2_display.print("SYOTA SANA:");
    tft.drawFastHLine(0, 19, 128, COLOR_GRAY);

    int visualLen = hpWordLen(hpInput);
    u8g2_display.setCursor(8, 62);
    if (visualLen == 0) {
      u8g2_display.setForegroundColor(COLOR_GRAY);
      u8g2_display.print("kirjoita sana");
    } else {
      u8g2_display.setForegroundColor(COLOR_YELLOW);
      u8g2_display.print(hpInput);
    }

    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(2, 85);
    u8g2_display.print("Kirjaimia: ");
    u8g2_display.print(visualLen);
    u8g2_display.setCursor(2, 113);
    u8g2_display.print("ENTER = valmis");
    u8g2_display.setCursor(2, 126);
    u8g2_display.print("DEL = poista");
    return;
  }

  // ── Handoff screen ───────────────────────────────────────────────────────
  if (hpPhase == HP_PASS) {
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(2, 15);
    u8g2_display.print("HIRSIPUU");
    tft.drawFastHLine(0, 19, 128, COLOR_GRAY);

    u8g2_display.setForegroundColor(COLOR_YELLOW);
    u8g2_display.setCursor(6, 52);
    u8g2_display.print("Anna laite");
    u8g2_display.setCursor(6, 68);
    u8g2_display.print("pelaajalle 2!");

    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(4, 108);
    u8g2_display.print("ENTER = aloita");
    return;
  }

  // ── Playing / Win / Lose ─────────────────────────────────────────────────
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(2, 13);
  u8g2_display.print("HIRSIPUU");
  u8g2_display.setForegroundColor(hpWrong >= 4 ? 0xF800 : COLOR_YELLOW);
  u8g2_display.setCursor(82, 13);
  u8g2_display.print(String(hpWrong) + "/" + String(HP_MAX_WRONG));
  tft.drawFastHLine(0, 16, 128, COLOR_GRAY);

  hpDrawGallows(hpWrong);

  // Word display — iterate logical characters to handle Ä/Ö
  int wlen   = hpWordLen(hpWord);
  int slotW  = (wlen <= 8) ? 11 : 9;
  int wordW  = wlen * slotW;
  int startX = (128 - wordW) / 2;
  int lineY  = 87;
  int letterY = 85;

  for (int i = 0; i < wlen; i++) {
    String letter = hpGetLetter(hpWord, i);
    int    sx     = startX + i * slotW;
    tft.drawFastHLine(sx, lineY, slotW - 3, COLOR_GRAY);
    if (hpIsGuessed(letter) || hpPhase == HP_WIN || hpPhase == HP_LOSE) {
      bool missed = (hpPhase == HP_LOSE && !hpIsGuessed(letter));
      u8g2_display.setForegroundColor(missed ? 0xF800 : COLOR_GREEN);
      u8g2_display.setCursor(sx, letterY);
      u8g2_display.print(letter);
    }
  }

  // Wrong letters row (only during active play)
  if (hpPhase == HP_PLAYING && hpWrong > 0) {
    String wrongL = "";
    int i = 0;
    while (i < (int)hpGuessed.length()) {
      int    len    = (((uint8_t)hpGuessed[i] & 0xE0) == 0xC0) ? 2 : 1;
      String letter = hpGuessed.substring(i, i + len);
      if (hpWord.indexOf(letter) < 0) { wrongL += letter; wrongL += ' '; }
      i += len;
    }
    u8g2_display.setForegroundColor(0xF800);
    u8g2_display.setCursor(2, 103);
    u8g2_display.print(wrongL);
  }

  // Win / Lose popup
  if (hpPhase == HP_WIN || hpPhase == HP_LOSE) {
    bool     won    = (hpPhase == HP_WIN);
    uint16_t popCol = won ? COLOR_GREEN : 0xF800;
    tft.fillRoundRect(8, 92, 112, 34, 4, COLOR_POPUP_BG);
    tft.drawRoundRect(8, 92, 112, 34, 4, popCol);
    u8g2_display.setForegroundColor(popCol);
    u8g2_display.setCursor(won ? 30 : 22, 109);
    u8g2_display.print(won ? "VOITTO!" : "HIRSIPUU!");
    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(14, 122);
    u8g2_display.print("ENTER = uusi");
  }
}

// --- Init (called once at boot) ---
void initHirsipuu() {
  hpPhase = HP_SUBMENU;
  hpSel   = 0;
}

// --- Setup (called when game is launched from menu) ---
void setupHirsipuu() {
  hpPhase   = HP_SUBMENU;
  hpSel     = 0;
  hpWord    = "";
  hpInput   = "";
  hpGuessed = "";
  hpWrong   = 0;
  drawHirsipuuBoard();
}

// --- Start a round with a given word ---
void hpStartGame(String word) {
  hpWord    = word;
  hpGuessed = "";
  hpWrong   = 0;
  hpPhase   = HP_PLAYING;
  drawHirsipuuBoard();
}

// --- Process one letter guess (String, 1 or 2 bytes) ---
void hpGuessLetter(String letter) {
  if (hpIsGuessed(letter)) return;
  hpGuessed += letter;
  if (hpWord.indexOf(letter) < 0) {
    hpWrong++;
    if (hpWrong >= HP_MAX_WRONG) hpPhase = HP_LOSE;
  } else {
    bool win = true;
    int  wlen = hpWordLen(hpWord);
    for (int i = 0; i < wlen; i++) {
      if (!hpIsGuessed(hpGetLetter(hpWord, i))) { win = false; break; }
    }
    if (win) hpPhase = HP_WIN;
  }
  drawHirsipuuBoard();
}

// --- Input handler ---
void handleHirsipuuInput(char c) {
  if (hpPhase == HP_SUBMENU) {
    if      (c == 0xB5) { hpSel = 0; drawHirsipuuBoard(); }
    else if (c == 0xB6) { hpSel = 1; drawHirsipuuBoard(); }
    else if (c == 0x0D || c == 0x0A) {
      if (hpSel == 0) {
        char buffer[24];
        strcpy_P(buffer, (char*)pgm_read_ptr(&(hp_words[random(0, HP_WORD_COUNT)])));
        hpStartGame(String(buffer));
      } else {
        hpInput = "";
        hpPhase = HP_INPUT;
        drawHirsipuuBoard();
      }
    }
    return;
  }

  if (hpPhase == HP_INPUT) {
    if (c == 0x0D || c == 0x0A) {
      if (hpWordLen(hpInput) >= 3) {
        hpPhase = HP_PASS;
        drawHirsipuuBoard();
      }
    } else if (c == 0x08) {
      if (hpInput.length() > 0) {
        // Remove last logical character (1 or 2 bytes)
        uint8_t lastByte = (uint8_t)hpInput[hpInput.length() - 1];
        int removeLen = ((lastByte & 0xC0) == 0x80) ? 2 : 1;
        hpInput = hpInput.substring(0, hpInput.length() - removeLen);
        drawHirsipuuBoard();
      }
    } else if (hpWordLen(hpInput) < 12) {
      // Same remapping as Sanuli: . → Ä, , → Ö
      if      (c == '.')                          { hpInput += "Ä"; drawHirsipuuBoard(); }
      else if (c == ',')                          { hpInput += "Ö"; drawHirsipuuBoard(); }
      else if (c >= 'a' && c <= 'z') { hpInput += (char)toupper((unsigned char)c); drawHirsipuuBoard(); }
      else if (c >= 'A' && c <= 'Z') { hpInput += c;                               drawHirsipuuBoard(); }
    }
    return;
  }

  if (hpPhase == HP_PASS) {
    if (c == 0x0D || c == 0x0A) hpStartGame(hpInput);
    return;
  }

  if (hpPhase == HP_WIN || hpPhase == HP_LOSE) {
    if (c == 0x0D || c == 0x0A) {
      hpPhase = HP_SUBMENU;
      hpSel   = 0;
      drawHirsipuuBoard();
    }
    return;
  }

  if (hpPhase == HP_PLAYING) {
    // Same remapping as Sanuli: . → Ä, , → Ö
    if      (c == '.')                { hpGuessLetter("Ä"); }
    else if (c == ',')                { hpGuessLetter("Ö"); }
    else if (c >= 'a' && c <= 'z')   { hpGuessLetter(String((char)(c - 32))); }
    else if (c >= 'A' && c <= 'Z')   { hpGuessLetter(String(c)); }
  }
}

#endif
