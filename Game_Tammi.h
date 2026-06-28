#ifndef GAME_TAMMI_H
#define GAME_TAMMI_H
#include "Globals.h"
#include "Comms.h"

// Checkers ("Tammi") on 8x8. Pieces on dark squares.
//   1 = white man, 2 = white king (side 0, bottom, moves up, moves first)
//   3 = black man, 4 = black king (side 1, top, moves down)
// Men move/capture diagonally forward; kings both ways (one square; jump two).
// Capturing is mandatory. (No multi-jump chaining in this version.)
int tamBoard[8][8];
int tamTurn = 0;                 // 0 = white, 1 = black
int tamCurR = 0, tamCurC = 0;
int tamSelR = -1, tamSelC = -1;
bool tamGameOver = false;
int  tamWinner = 0;              // side that won

#define TAM_MENU    0
#define TAM_PLAY    1
#define TAM_PAIRING 2
#define TAM_LOBBY   3
#define TAM_DISCONN 4
#define TAM_NET_STATE 1

int  tamPhase = TAM_MENU;
int  tamMode  = 0;               // 0 = 2 players, 1 = network, 2 = vs device
int  tamMenuSel = 0;
int  tamMyColor = 0;             // network: 0 white, 1 black
bool tamOwnerIsWhite = true;
unsigned long tamPairStart = 0;
int  tamBotDepth = 4;
int  tamBotBlunder = 0;

#define TAM_LIGHT 0xF7DE
#define TAM_DARK  0x34A6
#define TAM_SEL   0x07E0

// --- piece helpers ---
bool tamIsWhite(int p) { return p == 1 || p == 2; }
bool tamIsKing(int p)  { return p == 2 || p == 4; }
int  tamSide(int p)    { return (p == 1 || p == 2) ? 0 : ((p == 3 || p == 4) ? 1 : -1); }
bool tamDirOK(int p, int dr) {
  if (p == 2 || p == 4) return true;        // king: any direction
  if (p == 1) return dr == -1;              // white man: up
  if (p == 3) return dr == 1;               // black man: down
  return false;
}

bool tamInB(int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }

bool tamCanStep(int fr, int fc, int tr, int tc) {
  int p = tamBoard[fr][fc];
  if (p == 0) return false;
  int dr = tr - fr, dc = tc - fc;
  if (abs(dr) != 1 || abs(dc) != 1) return false;
  if (!tamInB(tr, tc) || tamBoard[tr][tc] != 0) return false;
  return tamDirOK(p, dr);
}

bool tamCanJump(int fr, int fc, int tr, int tc) {
  int p = tamBoard[fr][fc];
  if (p == 0) return false;
  int dr = tr - fr, dc = tc - fc;
  if (abs(dr) != 2 || abs(dc) != 2) return false;
  if (!tamInB(tr, tc) || tamBoard[tr][tc] != 0) return false;
  if (!tamDirOK(p, dr / 2)) return false;
  int mid = tamBoard[fr + dr / 2][fc + dc / 2];
  int ms = tamSide(mid);
  return ms != -1 && ms != tamSide(p);
}

bool tamHasJump(int color) {
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
    if (tamSide(tamBoard[r][c]) != color) continue;
    for (int dr = -2; dr <= 2; dr += 4) for (int dc = -2; dc <= 2; dc += 4)
      if (tamCanJump(r, c, r + dr, c + dc)) return true;
  }
  return false;
}

bool tamHasMove(int color) {
  if (tamHasJump(color)) return true;
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
    if (tamSide(tamBoard[r][c]) != color) continue;
    for (int dr = -1; dr <= 1; dr += 2) for (int dc = -1; dc <= 1; dc += 2)
      if (tamCanStep(r, c, r + dr, c + dc)) return true;
  }
  return false;
}

bool tamLegal(int fr, int fc, int tr, int tc, int color) {
  if (tamSide(tamBoard[fr][fc]) != color) return false;
  if (tamCanJump(fr, fc, tr, tc)) return true;
  if (!tamHasJump(color) && tamCanStep(fr, fc, tr, tc)) return true;
  return false;
}

// make/unmake (single jump, no chaining)
void tamMake(int fr, int fc, int tr, int tc, int& cap, int& capr, int& capc, int& orig) {
  orig = tamBoard[fr][fc];
  cap = 0; capr = -1; capc = -1;
  if (abs(tr - fr) == 2) { capr = (fr + tr) / 2; capc = (fc + tc) / 2; cap = tamBoard[capr][capc]; tamBoard[capr][capc] = 0; }
  tamBoard[tr][tc] = orig; tamBoard[fr][fc] = 0;
  if (orig == 1 && tr == 0) tamBoard[tr][tc] = 2;        // promote
  else if (orig == 3 && tr == 7) tamBoard[tr][tc] = 4;
}
void tamUnmake(int fr, int fc, int tr, int tc, int cap, int capr, int capc, int orig) {
  tamBoard[fr][fc] = orig; tamBoard[tr][tc] = 0;
  if (cap) tamBoard[capr][capc] = cap;
}

// --- Bot (negamax + alpha-beta) ---
int tamEvalWhite() {
  int s = 0;
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
    int p = tamBoard[r][c]; if (!p) continue;
    int v = tamIsKing(p) ? 180 : 100;
    if (p == 1) v += (7 - r);          // white man: closer to top = better
    else if (p == 3) v += r;           // black man: closer to bottom = better
    s += tamIsWhite(p) ? v : -v;
  }
  return s;
}
int tamEvalFor(int color) { return color == 0 ? tamEvalWhite() : -tamEvalWhite(); }

int tamNegamax(int depth, int alpha, int beta, int color) {
  if (depth == 0) return tamEvalFor(color);
  bool jumpMode = tamHasJump(color);
  int best = -2000000; bool any = false;
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
    if (tamSide(tamBoard[r][c]) != color) continue;
    for (int dr = -1; dr <= 1; dr += 2) for (int dc = -1; dc <= 1; dc += 2) {
      for (int step = 2; step >= 1; step--) {      // try jump (2) then step (1)
        if (step == 1 && jumpMode) continue;       // must jump if a jump exists
        int tr = r + dr * step, tc = c + dc * step;
        bool ok = (step == 2) ? tamCanJump(r, c, tr, tc) : tamCanStep(r, c, tr, tc);
        if (!ok) continue;
        int cap, capr, capc, orig; tamMake(r, c, tr, tc, cap, capr, capc, orig);
        int score = -tamNegamax(depth - 1, -beta, -alpha, 1 - color);
        tamUnmake(r, c, tr, tc, cap, capr, capc, orig);
        any = true;
        if (score > best) best = score;
        if (best > alpha) alpha = best;
        if (alpha >= beta) return best;
      }
    }
  }
  if (!any) return -1900000 + depth;   // no moves -> this side loses
  return best;
}

void tamBotMove() {   // bot plays black (side 1)
  bool jumpMode = tamHasJump(1);
  if (tamBotBlunder > 0 && (int)random(0, 100) < tamBotBlunder) {
    int count = 0;
    for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
      if (tamSide(tamBoard[r][c]) != 1) continue;
      for (int dr = -1; dr <= 1; dr += 2) for (int dc = -1; dc <= 1; dc += 2) {
        if (tamCanJump(r, c, r + 2*dr, c + 2*dc)) count++;
        else if (!jumpMode && tamCanStep(r, c, r + dr, c + dc)) count++;
      }
    }
    if (count == 0) return;
    int pick = random(0, count), idx = 0;
    for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
      if (tamSide(tamBoard[r][c]) != 1) continue;
      for (int dr = -1; dr <= 1; dr += 2) for (int dc = -1; dc <= 1; dc += 2) {
        int st = tamCanJump(r, c, r + 2*dr, c + 2*dc) ? 2 : ((!jumpMode && tamCanStep(r, c, r + dr, c + dc)) ? 1 : 0);
        if (!st) continue;
        if (idx == pick) { int cap, cr, cc, o; tamMake(r, c, r + dr*st, c + dc*st, cap, cr, cc, o); return; }
        idx++;
      }
    }
    return;
  }

  int bestScore = -2000000, bfr = -1, bfc = 0, btr = 0, btc = 0;
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
    if (tamSide(tamBoard[r][c]) != 1) continue;
    for (int dr = -1; dr <= 1; dr += 2) for (int dc = -1; dc <= 1; dc += 2) {
      for (int step = 2; step >= 1; step--) {
        if (step == 1 && jumpMode) continue;
        int tr = r + dr * step, tc = c + dc * step;
        bool ok = (step == 2) ? tamCanJump(r, c, tr, tc) : tamCanStep(r, c, tr, tc);
        if (!ok) continue;
        int cap, capr, capc, orig; tamMake(r, c, tr, tc, cap, capr, capc, orig);
        int score = -tamNegamax(tamBotDepth - 1, -2000000, 2000000, 0);
        tamUnmake(r, c, tr, tc, cap, capr, capc, orig);
        if (bfr < 0 || score > bestScore) { bestScore = score; bfr = r; bfc = c; btr = tr; btc = tc; }
      }
    }
    yield();
  }
  if (bfr >= 0) { int cap, cr, cc, o; tamMake(bfr, bfc, btr, btc, cap, cr, cc, o); }
}

// --- Screens ---
void tamDrawMenu() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("TAMMI");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);
  const char* opt[4] = { "HELPPO", "VAIKEA", "2 PELAAJAA", "2 LAITETTA" };
  for (int i = 0; i < 4; i++) {
    int y = 44 + i * 22;
    if (i == tamMenuSel) { tft.fillRect(0, y - 15, 128, 20, 0x18C3); u8g2_display.setForegroundColor(COLOR_GREEN); }
    else u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, y);
    u8g2_display.print(i == tamMenuSel ? ">" : " ");
    u8g2_display.print(opt[i]);
  }
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 126); u8g2_display.print("ENTER = aloita");
}

void tamDrawPairing() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("TAMMI");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(6, 58); u8g2_display.print("Etsitään toista");
  u8g2_display.setCursor(6, 78); u8g2_display.print("laitetta...");
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(6, 116); u8g2_display.print("ESC = peruuta");
}

void tamDrawLobby() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("TAMMI");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);
  if (commsLobbyOwner) {
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, 44); u8g2_display.print("Kumpi olet?");
    const char* opt[2] = { "VALKOINEN", "PUNAINEN" };
    for (int i = 0; i < 2; i++) {
      int y = 70 + i * 22;
      if (i == tamMenuSel) { tft.fillRect(0, y - 15, 128, 20, 0x18C3); u8g2_display.setForegroundColor(COLOR_GREEN); }
      else u8g2_display.setForegroundColor(COLOR_TEXT);
      u8g2_display.setCursor(6, y);
      u8g2_display.print(i == tamMenuSel ? ">" : " ");
      u8g2_display.print(opt[i]);
    }
    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(2, 124); u8g2_display.print("Valkoinen aloittaa");
  } else {
    u8g2_display.setForegroundColor(COLOR_GREEN);
    u8g2_display.setCursor(6, 56); u8g2_display.print("Yhdistetty!");
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, 84); u8g2_display.print("Toinen valitsee...");
  }
}

void tamDrawDisconn() {
  tft.fillRect(14, 40, 100, 48, COLOR_POPUP_BG);
  tft.drawRect(14, 40, 100, 48, COLOR_BORDER);
  u8g2_display.setForegroundColor(COLOR_RED);
  if (commsPartnerLeft) { u8g2_display.setCursor(24, 60); u8g2_display.print("Vastustaja"); u8g2_display.setCursor(24, 78); u8g2_display.print("poistui"); }
  else                  { u8g2_display.setCursor(24, 60); u8g2_display.print("Yhteys"); u8g2_display.setCursor(24, 78); u8g2_display.print("katkesi"); }
}

void drawTammiBoard() {
  if (tamPhase == TAM_MENU)    { tamDrawMenu();    return; }
  if (tamPhase == TAM_PAIRING) { tamDrawPairing(); return; }
  if (tamPhase == TAM_LOBBY)   { tamDrawLobby();   return; }

  bool flip = (tamMode == 1 && tamMyColor == 1);
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
    int sx = flip ? (7 - c) : c;
    int sy = flip ? (7 - r) : r;
    int x = sx * 16, y = sy * 16;
    uint16_t bg = ((r + c) % 2 == 0) ? TAM_LIGHT : TAM_DARK;
    if (tamSelR == r && tamSelC == c) bg = TAM_SEL;
    tft.fillRect(x, y, 16, 16, bg);
    int p = tamBoard[r][c];
    if (p) {
      bool w = tamIsWhite(p);
      tft.fillCircle(x + 8, y + 8, 6, w ? 0xFFFF : 0xF800);
      if (tamIsKing(p)) tft.fillCircle(x + 8, y + 8, 2, w ? 0xF800 : 0xFFFF);
    }
  }

  int csR = flip ? (7 - tamCurR) : tamCurR;
  int csC = flip ? (7 - tamCurC) : tamCurC;
  tft.drawRect(csC * 16, csR * 16, 16, 16, COLOR_YELLOW);
  tft.drawRect(csC * 16 + 1, csR * 16 + 1, 14, 14, COLOR_YELLOW);

  if (tamPhase == TAM_DISCONN) { tamDrawDisconn(); return; }

  if (tamGameOver) {
    tft.fillRect(14, 40, 100, 48, COLOR_POPUP_BG);
    tft.drawRect(14, 40, 100, 48, COLOR_BORDER);
    if (tamMode == 0) {
      u8g2_display.setForegroundColor(COLOR_TEXT);
      u8g2_display.setCursor(20, 62); u8g2_display.print(tamWinner == 0 ? "VALKOINEN" : "PUNAINEN");
      u8g2_display.setCursor(35, 82); u8g2_display.print("VOITTI!");
    } else {
      int me = (tamMode == 2) ? 0 : tamMyColor;
      if (tamWinner == me) { u8g2_display.setForegroundColor(COLOR_GREEN); u8g2_display.setCursor(30, 70); u8g2_display.print("VOITIT!"); }
      else                 { u8g2_display.setForegroundColor(COLOR_RED);   u8g2_display.setCursor(30, 70); u8g2_display.print("HÄVISIT"); }
    }
  }
}

// --- Network serialization ---
void tamSendState() {
  uint8_t b[69];
  b[0] = TAM_NET_STATE;
  b[1] = tamOwnerIsWhite ? 1 : 0;
  int k = 2;
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) b[k++] = (uint8_t)tamBoard[r][c];
  b[66] = (uint8_t)tamTurn;
  b[67] = tamGameOver ? 1 : 0;
  b[68] = (uint8_t)tamWinner;
  commsSend(b, 69);
}

void tamRecvState(const uint8_t* b) {
  tamOwnerIsWhite = (b[1] != 0);
  tamMyColor = commsLobbyOwner ? (tamOwnerIsWhite ? 0 : 1) : (tamOwnerIsWhite ? 1 : 0);
  int k = 2;
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) tamBoard[r][c] = b[k++];
  tamTurn = b[66];
  tamGameOver = (b[67] != 0);
  tamWinner = b[68];
  tamSelR = -1; tamSelC = -1;
  if (tamPhase == TAM_LOBBY) { tamCurR = 0; tamCurC = 0; }
  tamPhase = TAM_PLAY;
  drawTammiBoard();
}

// --- Flow ---
void tamInitBoard() {
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) tamBoard[r][c] = 0;
  for (int r = 0; r < 3; r++) for (int c = 0; c < 8; c++) if ((r + c) % 2 == 1) tamBoard[r][c] = 3;  // black top
  for (int r = 5; r < 8; r++) for (int c = 0; c < 8; c++) if ((r + c) % 2 == 1) tamBoard[r][c] = 1;  // white bottom
}

void setupTammi() {
  tamPhase = TAM_MENU; tamMenuSel = 0;
  drawTammiBoard();
}

void tamStartGame() {
  tamInitBoard();
  tamTurn = 0; tamCurR = 0; tamCurC = 0; tamSelR = -1; tamSelC = -1; tamGameOver = false; tamWinner = 0;
  tamPhase = TAM_PLAY;
  drawTammiBoard();
}

void tamStartNetGame() {
  tamInitBoard();
  tamTurn = 0; tamCurR = 0; tamCurC = 0; tamSelR = -1; tamSelC = -1; tamGameOver = false; tamWinner = 0;
  tamPhase = TAM_PLAY;
  drawTammiBoard();
  tamSendState();
}

void tammiOnExit() {
  if (tamMode == 1) { commsEnd(); tamMode = 0; tamPhase = TAM_MENU; }
}

void updateTammi() {
  if (tamMode != 1) return;
  commsTick();

  if ((commsPartnerLeft || commsLinkLost) && (tamPhase == TAM_LOBBY || tamPhase == TAM_PLAY)) {
    tamPhase = TAM_DISCONN; drawTammiBoard(); return;
  }
  if (tamPhase == TAM_PAIRING) {
    if (commsPaired) { tamPhase = TAM_LOBBY; tamMenuSel = 0; drawTammiBoard(); }
    else if (millis() - tamPairStart > 30000) { tammiOnExit(); setupTammi(); }
    return;
  }
  if (tamPhase == TAM_LOBBY || tamPhase == TAM_PLAY) {
    uint8_t buf[80]; uint8_t len;
    if (commsPoll(buf, &len) && len >= 69 && buf[0] == TAM_NET_STATE) tamRecvState(buf);
  }
}

// applies a (legal) move from selection to (tamCurR,tamCurC) and advances the game
void tamCommitMove() {
  int cap, cr, cc, o;
  tamMake(tamSelR, tamSelC, tamCurR, tamCurC, cap, cr, cc, o);
  tamSelR = -1; tamSelC = -1;
  tamTurn = 1 - tamTurn;
  if (!tamHasMove(tamTurn)) { tamGameOver = true; tamWinner = 1 - tamTurn; }

  if (tamMode == 1) tamSendState();
  else if (tamMode == 2 && !tamGameOver) {
    tamBotMove();
    tamTurn = 1 - tamTurn;
    if (!tamHasMove(tamTurn)) { tamGameOver = true; tamWinner = 1 - tamTurn; }
  }
}

void handleTammiInput(char c) {
  if (tamPhase == TAM_MENU) {
    if      (c == 0xB5) { if (tamMenuSel > 0) tamMenuSel--; drawTammiBoard(); }
    else if (c == 0xB6) { if (tamMenuSel < 3) tamMenuSel++; drawTammiBoard(); }
    else if (c == 0x0D || c == 0x0A) {
      if      (tamMenuSel == 0) { tamMode = 2; tamBotDepth = 3; tamBotBlunder = 25; tamStartGame(); }
      else if (tamMenuSel == 1) { tamMode = 2; tamBotDepth = 6; tamBotBlunder = 0;  tamStartGame(); }
      else if (tamMenuSel == 2) { tamMode = 0; tamStartGame(); }
      else                      { tamMode = 1; commsBegin(); tamPhase = TAM_PAIRING; tamPairStart = millis(); drawTammiBoard(); }
    }
    return;
  }

  if (tamPhase == TAM_PAIRING || tamPhase == TAM_DISCONN) return;

  if (tamPhase == TAM_LOBBY) {
    if (commsLobbyOwner) {
      if      (c == 0xB5) { tamMenuSel = 0; drawTammiBoard(); }
      else if (c == 0xB6) { tamMenuSel = 1; drawTammiBoard(); }
      else if (c == 0x0D || c == 0x0A) {
        tamOwnerIsWhite = (tamMenuSel == 0);
        tamMyColor = tamOwnerIsWhite ? 0 : 1;
        tamStartNetGame();
      }
    }
    return;
  }

  // TAM_PLAY
  if (tamGameOver) {
    if (c == 0x0D || c == 0x0A) {
      if (tamMode == 1) { if (commsLobbyOwner) tamStartNetGame(); }
      else tamStartGame();
    }
    return;
  }

  if (tamMode == 1 && tamMyColor == 1) {   // black's board is flipped -> invert arrows
    if      (c == 0xB4) c = 0xB7;
    else if (c == 0xB7) c = 0xB4;
    else if (c == 0xB5) c = 0xB6;
    else if (c == 0xB6) c = 0xB5;
  }

  if      (c == 0xB4) tamCurC = (tamCurC > 0) ? tamCurC - 1 : 7;
  else if (c == 0xB7) tamCurC = (tamCurC < 7) ? tamCurC + 1 : 0;
  else if (c == 0xB5) tamCurR = (tamCurR > 0) ? tamCurR - 1 : 7;
  else if (c == 0xB6) tamCurR = (tamCurR < 7) ? tamCurR + 1 : 0;
  else if (c == 0x0D || c == 0x0A) {
    if (tamMode == 1 && tamTurn != tamMyColor) { drawTammiBoard(); return; }   // not your turn
    if (tamSelR == -1) {
      if (tamSide(tamBoard[tamCurR][tamCurC]) == tamTurn) { tamSelR = tamCurR; tamSelC = tamCurC; }
    } else if (tamSelR == tamCurR && tamSelC == tamCurC) {
      tamSelR = -1; tamSelC = -1;
    } else if (tamLegal(tamSelR, tamSelC, tamCurR, tamCurC, tamTurn)) {
      tamCommitMove();
    } else if (tamSide(tamBoard[tamCurR][tamCurC]) == tamTurn) {
      tamSelR = tamCurR; tamSelC = tamCurC;
    }
  }
  drawTammiBoard();
}

#endif
