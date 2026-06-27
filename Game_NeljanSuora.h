#ifndef GAME_NELJANSUORA_H
#define GAME_NELJANSUORA_H
#include "Globals.h"
#include "Comms.h"

// Connect Four ("Neljän suora").
//  - HELPPO / VAIKEA : vs the device (red = you)
//  - 2 PELAAJAA      : red vs yellow on one device
//  - 2 LAITETTA      : red vs yellow over ESP-NOW on two devices
// First to four in a row wins.

// --- Layout ---
#define C4_COLS  7
#define C4_ROWS  6
#define C4_CELL  17
#define C4_X0     4
#define C4_Y0    14
#define C4_R      7

// --- Colors ---
#define C4_COL_BOARD 0x195F
#define C4_COL_P1    0xF800    // player 1 / you: red
#define C4_COL_P2    0xFFE0    // player 2 / device / opponent: yellow

// --- Phases ---
#define C4_MENU    0
#define C4_PLAY    1
#define C4_PAIRING 2           // network: looking for the other device
#define C4_LOBBY   3           // network: owner picks who starts
#define C4_DISCONN 4           // network: link lost / opponent left

#define C4_NET_STATE 1         // network message subtype (room for more later)

// --- State ---
int  c4board[C4_ROWS][C4_COLS];   // 0 empty, 1 red, 2 yellow
int  c4cursorCol = 3;
int  c4phase     = C4_MENU;
int  c4mode      = 0;             // 0 = vs device, 1 = 2 players (one device), 2 = network
int  c4SearchDepth = 6;           // AI lookahead (set by difficulty)
int  c4BlunderPct  = 0;           // AI random-move chance % (easy mode)
int  c4menuSel   = 0;
int  c4turn      = 1;             // whose turn: 1 (red) or 2 (yellow)
bool c4GameOver  = false;
int  c4winner    = 0;             // 0 none/draw, 1 red, 2 yellow
int  c4Wins      = 0;             // your wins vs the device, saved
// network only:
int  c4myPlayer    = 1;           // which colour this device controls (1 red, 2 yellow)
bool c4ownerIsRed  = true;        // lobby owner's choice (red moves first)
unsigned long c4pairStart = 0;

// --- Board helpers ---
int c4Drop(int col, int p) {
  for (int r = C4_ROWS - 1; r >= 0; r--)
    if (c4board[r][col] == 0) { c4board[r][col] = p; return r; }
  return -1;
}

bool c4BoardFull() {
  for (int c = 0; c < C4_COLS; c++) if (c4board[0][c] == 0) return false;
  return true;
}

bool c4HasFour(int p) {
  for (int r = 0; r < C4_ROWS; r++)
    for (int c = 0; c < C4_COLS; c++) {
      if (c4board[r][c] != p) continue;
      if (c <= C4_COLS - 4 &&
          c4board[r][c+1] == p && c4board[r][c+2] == p && c4board[r][c+3] == p) return true;
      if (r <= C4_ROWS - 4 &&
          c4board[r+1][c] == p && c4board[r+2][c] == p && c4board[r+3][c] == p) return true;
      if (r <= C4_ROWS - 4 && c <= C4_COLS - 4 &&
          c4board[r+1][c+1] == p && c4board[r+2][c+2] == p && c4board[r+3][c+3] == p) return true;
      if (r >= 3 && c <= C4_COLS - 4 &&
          c4board[r-1][c+1] == p && c4board[r-2][c+2] == p && c4board[r-3][c+3] == p) return true;
    }
  return false;
}

// --- AI (negamax + alpha-beta), depth/blunder set by difficulty ---
#define C4_WIN_SCORE 100000
static const int c4Order[C4_COLS] = {3, 2, 4, 1, 5, 0, 6};

int c4DropRow(int col) {
  for (int r = C4_ROWS - 1; r >= 0; r--) if (c4board[r][col] == 0) return r;
  return -1;
}

bool c4WinAt(int r, int c, int p) {
  static const int dirs[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
  for (int d = 0; d < 4; d++) {
    int dr = dirs[d][0], dc = dirs[d][1], cnt = 1;
    for (int s = 1; s < 4; s++) { int rr = r + dr*s, cc = c + dc*s; if (rr<0||rr>=C4_ROWS||cc<0||cc>=C4_COLS||c4board[rr][cc]!=p) break; cnt++; }
    for (int s = 1; s < 4; s++) { int rr = r - dr*s, cc = c - dc*s; if (rr<0||rr>=C4_ROWS||cc<0||cc>=C4_COLS||c4board[rr][cc]!=p) break; cnt++; }
    if (cnt >= 4) return true;
  }
  return false;
}

int c4Window(int r, int c, int dr, int dc, int p) {
  int cp = 0, co = 0;
  for (int i = 0; i < 4; i++) { int v = c4board[r + dr*i][c + dc*i]; if (v == p) cp++; else if (v != 0) co++; }
  if (co > 0) return 0;
  return (cp == 3) ? 50 : (cp == 2) ? 10 : (cp == 1) ? 1 : 0;
}

int c4ScoreFor(int p) {
  int s = 0;
  for (int r = 0; r < C4_ROWS; r++) if (c4board[r][3] == p) s += 3;
  for (int r = 0; r < C4_ROWS; r++) for (int c = 0; c <= C4_COLS-4; c++) s += c4Window(r, c, 0, 1, p);
  for (int c = 0; c < C4_COLS; c++) for (int r = 0; r <= C4_ROWS-4; r++) s += c4Window(r, c, 1, 0, p);
  for (int r = 0; r <= C4_ROWS-4; r++) for (int c = 0; c <= C4_COLS-4; c++) s += c4Window(r, c, 1, 1, p);
  for (int r = 3; r < C4_ROWS; r++) for (int c = 0; c <= C4_COLS-4; c++) s += c4Window(r, c, -1, 1, p);
  return s;
}

int c4Eval(int player) { return c4ScoreFor(player) - c4ScoreFor(player == 1 ? 2 : 1); }

int c4Negamax(int depth, int alpha, int beta, int player) {
  if (c4BoardFull()) return 0;
  if (depth == 0) return c4Eval(player);
  int opp = (player == 1) ? 2 : 1, best = -2000000;
  for (int k = 0; k < C4_COLS; k++) {
    int col = c4Order[k], r = c4DropRow(col);
    if (r < 0) continue;
    c4board[r][col] = player;
    int score = c4WinAt(r, col, player) ? (C4_WIN_SCORE - (c4SearchDepth - depth))
                                        : -c4Negamax(depth - 1, -beta, -alpha, opp);
    c4board[r][col] = 0;
    if (score > best) best = score;
    if (best > alpha) alpha = best;
    if (alpha >= beta) break;
  }
  return best;
}

int c4AIMove() {
  if (c4BlunderPct > 0 && (int)random(0, 100) < c4BlunderPct) {
    int legal[C4_COLS], n = 0;
    for (int c = 0; c < C4_COLS; c++) if (c4DropRow(c) >= 0) legal[n++] = c;
    if (n > 0) return legal[random(0, n)];
  }
  int bestScore = -2000000, pool[C4_COLS], pn = 0;
  for (int k = 0; k < C4_COLS; k++) {
    int col = c4Order[k], r = c4DropRow(col);
    if (r < 0) continue;
    c4board[r][col] = 2;
    int score = c4WinAt(r, col, 2) ? C4_WIN_SCORE
                                   : -c4Negamax(c4SearchDepth - 1, -2000000, 2000000, 1);
    c4board[r][col] = 0;
    if (score > bestScore) { bestScore = score; pn = 0; pool[pn++] = col; }
    else if (score == bestScore) pool[pn++] = col;
  }
  return pool[pn ? random(0, pn) : 0];
}

// --- Screens ---
void c4DrawMenu() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("NELJÄN SUORA");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);

  const char* opt[4] = { "HELPPO", "VAIKEA", "2 PELAAJAA", "2 LAITETTA" };
  for (int i = 0; i < 4; i++) {
    int y = 44 + i * 22;
    if (i == c4menuSel) {
      tft.fillRect(0, y - 15, 128, 20, 0x18C3);
      u8g2_display.setForegroundColor(COLOR_GREEN);
    } else {
      u8g2_display.setForegroundColor(COLOR_TEXT);
    }
    u8g2_display.setCursor(6, y);
    u8g2_display.print(i == c4menuSel ? ">" : " ");
    u8g2_display.print(opt[i]);
  }
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 126); u8g2_display.print("ENTER = aloita");
}

void c4DrawPairing() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("2 LAITETTA");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(6, 58); u8g2_display.print("Etsitään toista");
  u8g2_display.setCursor(6, 78); u8g2_display.print("laitetta...");
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(6, 116); u8g2_display.print("ESC = peruuta");
}

void c4DrawLobby() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("2 LAITETTA");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);

  if (commsLobbyOwner) {
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, 44); u8g2_display.print("Kuka aloittaa?");
    const char* opt[2] = { "MINÄ ALOITAN", "TOINEN ALOITTAA" };
    for (int i = 0; i < 2; i++) {
      int y = 72 + i * 24;
      if (i == c4menuSel) { tft.fillRect(0, y - 15, 128, 20, 0x18C3); u8g2_display.setForegroundColor(COLOR_GREEN); }
      else u8g2_display.setForegroundColor(COLOR_TEXT);
      u8g2_display.setCursor(6, y);
      u8g2_display.print(i == c4menuSel ? ">" : " ");
      u8g2_display.print(opt[i]);
    }
    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(2, 126); u8g2_display.print("ENTER = aloita");
  } else {
    u8g2_display.setForegroundColor(COLOR_GREEN);
    u8g2_display.setCursor(6, 56); u8g2_display.print("Yhdistetty!");
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, 84); u8g2_display.print("Toinen valitsee");
    u8g2_display.setCursor(6, 104); u8g2_display.print("aloittajan...");
  }
}

void c4DrawDisconn() {
  tft.fillRect(8, 44, 112, 48, COLOR_POPUP_BG);
  tft.drawRect(8, 44, 112, 48, COLOR_BORDER);
  u8g2_display.setForegroundColor(COLOR_RED);
  if (commsPartnerLeft) { u8g2_display.setCursor(16, 64); u8g2_display.print("Vastustaja"); u8g2_display.setCursor(16, 80); u8g2_display.print("poistui"); }
  else                  { u8g2_display.setCursor(16, 64); u8g2_display.print("Yhteys"); u8g2_display.setCursor(16, 80); u8g2_display.print("katkesi"); }
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(16, 104); u8g2_display.print("ESC = valikkoon");
}

void drawNeljanSuora() {
  if (c4phase == C4_MENU)    { c4DrawMenu();    return; }
  if (c4phase == C4_PAIRING) { c4DrawPairing(); return; }
  if (c4phase == C4_LOBBY)   { c4DrawLobby();   return; }

  tft.fillScreen(COLOR_BG);

  // Drop preview disc (only when it's your move)
  if (!c4GameOver && (c4mode != 2 || c4turn == c4myPlayer)) {
    int cx = C4_X0 + c4cursorCol * C4_CELL + C4_CELL / 2;
    uint16_t pc = (c4mode == 2) ? ((c4myPlayer == 1) ? C4_COL_P1 : C4_COL_P2)
                                : ((c4turn == 1) ? C4_COL_P1 : C4_COL_P2);
    tft.fillCircle(cx, 7, 4, pc);
  }

  // Board + discs
  tft.fillRect(C4_X0, C4_Y0, C4_COLS * C4_CELL, C4_ROWS * C4_CELL, C4_COL_BOARD);
  for (int r = 0; r < C4_ROWS; r++)
    for (int c = 0; c < C4_COLS; c++) {
      int cx = C4_X0 + c * C4_CELL + C4_CELL / 2;
      int cy = C4_Y0 + r * C4_CELL + C4_CELL / 2;
      uint16_t col = (c4board[r][c] == 1) ? C4_COL_P1 :
                     (c4board[r][c] == 2) ? C4_COL_P2 : COLOR_BG;
      tft.fillCircle(cx, cy, C4_R, col);
    }

  if (c4phase == C4_DISCONN) { c4DrawDisconn(); return; }

  if (c4GameOver) {
    tft.fillRect(8, 44, 112, 56, COLOR_POPUP_BG);
    tft.drawRect(8, 44, 112, 56, COLOR_BORDER);

    // Line 1: result
    if (c4winner == 0) {
      u8g2_display.setForegroundColor(COLOR_TEXT); u8g2_display.setCursor(14, 62); u8g2_display.print("TASAPELI");
    } else if (c4mode == 0) {
      if (c4winner == 1) { u8g2_display.setForegroundColor(COLOR_GREEN);  u8g2_display.setCursor(14, 62); u8g2_display.print("VOITIT!"); }
      else               { u8g2_display.setForegroundColor(COLOR_YELLOW); u8g2_display.setCursor(14, 62); u8g2_display.print("KONE VOITTI"); }
    } else if (c4mode == 2) {
      if (c4winner == c4myPlayer) { u8g2_display.setForegroundColor(COLOR_GREEN); u8g2_display.setCursor(14, 62); u8g2_display.print("VOITIT!"); }
      else                        { u8g2_display.setForegroundColor(COLOR_RED);   u8g2_display.setCursor(14, 62); u8g2_display.print("HÄVISIT"); }
    } else {
      u8g2_display.setForegroundColor(c4winner == 1 ? C4_COL_P1 : C4_COL_P2);
      u8g2_display.setCursor(14, 62); u8g2_display.print(c4winner == 1 ? "PUNAINEN" : "KELTAINEN");
    }

    // Line 2: context
    u8g2_display.setForegroundColor(COLOR_TEXT);
    if (c4mode == 0) { u8g2_display.setCursor(14, 79); u8g2_display.print("Voitot: "); u8g2_display.print(c4Wins); }
    else if (c4mode == 1 && c4winner != 0) { u8g2_display.setCursor(14, 79); u8g2_display.print("VOITTI!"); }

    // Line 3: how to continue
    u8g2_display.setForegroundColor(COLOR_TEXT);
    if (c4mode == 2 && !commsLobbyOwner) { u8g2_display.setCursor(14, 95); u8g2_display.print("ESC = valikkoon"); }
    else                                  { u8g2_display.setCursor(14, 95); u8g2_display.print("ENTER = uusi"); }
  }
}

// --- Network serialization (whole board each move) ---
void c4SendState() {
  uint8_t b[47];
  b[0] = C4_NET_STATE;
  b[1] = c4ownerIsRed ? 1 : 0;
  int k = 2;
  for (int r = 0; r < C4_ROWS; r++) for (int c = 0; c < C4_COLS; c++) b[k++] = (uint8_t)c4board[r][c];
  b[44] = (uint8_t)c4turn;
  b[45] = c4GameOver ? 1 : 0;
  b[46] = (uint8_t)c4winner;
  commsSend(b, 47);
}

void c4RecvState(const uint8_t* b) {
  c4ownerIsRed = (b[1] != 0);
  c4myPlayer = commsLobbyOwner ? (c4ownerIsRed ? 1 : 2) : (c4ownerIsRed ? 2 : 1);
  int k = 2;
  for (int r = 0; r < C4_ROWS; r++) for (int c = 0; c < C4_COLS; c++) c4board[r][c] = b[k++];
  c4turn = b[44];
  c4GameOver = (b[45] != 0);
  c4winner = b[46];
  if (c4phase == C4_LOBBY) c4cursorCol = 3;
  c4phase = C4_PLAY;
  drawNeljanSuora();
}

// --- Game flow ---
void initNeljanSuora() {
  Preferences p; p.begin("neljansuora", true);
  c4Wins = p.getInt("wins", 0);
  p.end();
}

void setupNeljanSuora() {     // launched from main menu -> show the chooser
  c4phase = C4_MENU;
  c4menuSel = 0;
  drawNeljanSuora();
}

void c4StartGame() {          // local modes (vs device / 2 players)
  for (int r = 0; r < C4_ROWS; r++)
    for (int c = 0; c < C4_COLS; c++) c4board[r][c] = 0;
  c4cursorCol = 3; c4turn = 1; c4GameOver = false; c4winner = 0;
  c4phase = C4_PLAY;
  drawNeljanSuora();
}

void c4StartNetGame() {       // owner starts/restarts a network game
  for (int r = 0; r < C4_ROWS; r++)
    for (int c = 0; c < C4_COLS; c++) c4board[r][c] = 0;
  c4cursorCol = 3; c4turn = 1; c4GameOver = false; c4winner = 0;
  c4phase = C4_PLAY;
  drawNeljanSuora();
  c4SendState();              // moves the other device into play too
}

void c4SaveWin() {
  c4Wins++;
  Preferences p; p.begin("neljansuora", false);
  p.putInt("wins", c4Wins);
  p.end();
}

void c4DoMove(int col) {
  if (c4Drop(col, c4turn) < 0) return;          // column full

  bool ended = false;
  if (c4HasFour(c4turn)) {
    c4GameOver = true; c4winner = c4turn; ended = true;
    if (c4mode == 0 && c4turn == 1) c4SaveWin();
  } else if (c4BoardFull()) {
    c4GameOver = true; c4winner = 0; ended = true;
  }

  if (!ended) {
    if (c4mode == 0) {                          // device replies
      c4turn = 2;
      c4Drop(c4AIMove(), 2);
      if (c4HasFour(2)) { c4GameOver = true; c4winner = 2; }
      else if (c4BoardFull()) { c4GameOver = true; c4winner = 0; }
      else c4turn = 1;
    } else {                                    // hotseat or network: pass turn
      c4turn = (c4turn == 1) ? 2 : 1;
    }
  }

  if (c4mode == 2) c4SendState();               // network: sync after every local change
  drawNeljanSuora();
}

void neljanSuoraOnExit() {    // called by the .ino when leaving via ESC
  if (c4mode == 2) { commsEnd(); c4mode = 0; c4phase = C4_MENU; }
}

void updateNeljanSuora() {    // called every loop; only does work in network mode
  if (c4mode != 2) return;
  commsTick();

  if ((commsPartnerLeft || commsLinkLost) && (c4phase == C4_LOBBY || c4phase == C4_PLAY)) {
    c4phase = C4_DISCONN;
    drawNeljanSuora();
    return;
  }

  if (c4phase == C4_PAIRING) {
    if (commsPaired) { c4phase = C4_LOBBY; c4menuSel = 0; drawNeljanSuora(); }
    else if (millis() - c4pairStart > 30000) { neljanSuoraOnExit(); setupNeljanSuora(); }
    return;
  }

  if (c4phase == C4_LOBBY || c4phase == C4_PLAY) {
    uint8_t buf[64]; uint8_t len;
    if (commsPoll(buf, &len) && len >= 47 && buf[0] == C4_NET_STATE) c4RecvState(buf);
  }
}

void handleNeljanSuoraInput(char c) {
  if (c4phase == C4_MENU) {
    if      (c == 0xB5) { if (c4menuSel > 0) c4menuSel--; drawNeljanSuora(); }
    else if (c == 0xB6) { if (c4menuSel < 3) c4menuSel++; drawNeljanSuora(); }
    else if (c == 0x0D || c == 0x0A) {
      if      (c4menuSel == 0) { c4mode = 0; c4SearchDepth = 2; c4BlunderPct = 35; c4StartGame(); }  // HELPPO
      else if (c4menuSel == 1) { c4mode = 0; c4SearchDepth = 6; c4BlunderPct = 0;  c4StartGame(); }  // VAIKEA
      else if (c4menuSel == 2) { c4mode = 1; c4StartGame(); }                                        // 2 PELAAJAA
      else                     { c4mode = 2; commsBegin(); c4phase = C4_PAIRING; c4pairStart = millis(); drawNeljanSuora(); }  // 2 LAITETTA
    }
    return;
  }

  if (c4phase == C4_PAIRING || c4phase == C4_DISCONN) return;   // ESC (global) handles leaving

  if (c4phase == C4_LOBBY) {
    if (commsLobbyOwner) {
      if      (c == 0xB5) { c4menuSel = 0; drawNeljanSuora(); }
      else if (c == 0xB6) { c4menuSel = 1; drawNeljanSuora(); }
      else if (c == 0x0D || c == 0x0A) {
        c4ownerIsRed = (c4menuSel == 0);
        c4myPlayer = c4ownerIsRed ? 1 : 2;
        c4StartNetGame();
      }
    }
    return;
  }

  // C4_PLAY
  if (c4GameOver) {
    if (c == 0x0D || c == 0x0A) {
      if (c4mode == 2) { if (commsLobbyOwner) c4StartNetGame(); }   // owner rematch
      else c4StartGame();
    }
    return;
  }
  if (c == 0xB4) {
    c4cursorCol = (c4cursorCol > 0) ? c4cursorCol - 1 : C4_COLS - 1; drawNeljanSuora();
  } else if (c == 0xB7) {
    c4cursorCol = (c4cursorCol < C4_COLS - 1) ? c4cursorCol + 1 : 0; drawNeljanSuora();
  } else if (c == 0x0D || c == 0x0A || c == 0xB6) {
    if (c4mode == 2 && c4turn != c4myPlayer) return;               // not your turn
    c4DoMove(c4cursorCol);
  }
}

#endif
