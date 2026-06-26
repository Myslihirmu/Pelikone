#ifndef GAME_NELJANSUORA_H
#define GAME_NELJANSUORA_H
#include "Globals.h"

// Connect Four ("Neljän suora").
//  - 1 player: you (red) vs the device (yellow)
//  - 2 players: red vs yellow, taking turns on the same device
// Drop a disc into a column; first to get four in a row wins.

// --- Layout ---
#define C4_COLS  7
#define C4_ROWS  6
#define C4_CELL  17
#define C4_X0     4            // 4 + 7*17 = 123 (centred on 128)
#define C4_Y0    14            // top 0..13 reserved for the drop preview
#define C4_R      7            // disc radius

// --- Colors ---
#define C4_COL_BOARD 0x195F    // board blue
#define C4_COL_P1    0xF800    // player 1 / you: red
#define C4_COL_P2    0xFFE0    // player 2 / device: yellow

// --- Phases ---
#define C4_MENU  0             // choosing 1 or 2 players
#define C4_PLAY  1

// --- State ---
int  c4board[C4_ROWS][C4_COLS];   // 0 empty, 1 red, 2 yellow
int  c4cursorCol = 3;
int  c4phase     = C4_MENU;
int  c4mode      = 0;             // 0 = 1 player (vs device), 1 = 2 players
int  c4SearchDepth = 6;           // AI lookahead in plies (set by difficulty)
int  c4BlunderPct  = 0;           // AI random-move chance % (easy mode)
int  c4menuSel   = 0;
int  c4turn      = 1;             // whose turn: 1 (red) or 2 (yellow)
bool c4GameOver  = false;
int  c4winner    = 0;             // 0 none/draw, 1 red, 2 yellow
int  c4Wins      = 0;             // your wins vs the device, saved

// --- Board helpers ---
int c4Drop(int col, int p) {                 // returns the row used, or -1 if full
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
          c4board[r][c+1] == p && c4board[r][c+2] == p && c4board[r][c+3] == p) return true;            // -
      if (r <= C4_ROWS - 4 &&
          c4board[r+1][c] == p && c4board[r+2][c] == p && c4board[r+3][c] == p) return true;            // |
      if (r <= C4_ROWS - 4 && c <= C4_COLS - 4 &&
          c4board[r+1][c+1] == p && c4board[r+2][c+2] == p && c4board[r+3][c+3] == p) return true;      // diag down-right
      if (r >= 3 && c <= C4_COLS - 4 &&
          c4board[r-1][c+1] == p && c4board[r-2][c+2] == p && c4board[r-3][c+3] == p) return true;      // diag up-right
    }
  return false;
}

// --- AI: negamax search with alpha-beta pruning (1-player mode). Depth/blunder set by difficulty. ---
#define C4_WIN_SCORE 100000
static const int c4Order[C4_COLS] = {3, 2, 4, 1, 5, 0, 6};   // search centre first (better pruning)

int c4DropRow(int col) {                    // row a disc would land in, or -1 if full (no placing)
  for (int r = C4_ROWS - 1; r >= 0; r--) if (c4board[r][col] == 0) return r;
  return -1;
}

bool c4WinAt(int r, int c, int p) {         // does the piece at (r,c) complete a four?
  static const int dirs[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
  for (int d = 0; d < 4; d++) {
    int dr = dirs[d][0], dc = dirs[d][1], cnt = 1;
    for (int s = 1; s < 4; s++) { int rr = r + dr*s, cc = c + dc*s; if (rr<0||rr>=C4_ROWS||cc<0||cc>=C4_COLS||c4board[rr][cc]!=p) break; cnt++; }
    for (int s = 1; s < 4; s++) { int rr = r - dr*s, cc = c - dc*s; if (rr<0||rr>=C4_ROWS||cc<0||cc>=C4_COLS||c4board[rr][cc]!=p) break; cnt++; }
    if (cnt >= 4) return true;
  }
  return false;
}

int c4Window(int r, int c, int dr, int dc, int p) {   // score one 4-cell line for player p
  int cp = 0, co = 0;
  for (int i = 0; i < 4; i++) { int v = c4board[r + dr*i][c + dc*i]; if (v == p) cp++; else if (v != 0) co++; }
  if (co > 0) return 0;                     // blocked by opponent -> no potential
  return (cp == 3) ? 50 : (cp == 2) ? 10 : (cp == 1) ? 1 : 0;
}

int c4ScoreFor(int p) {
  int s = 0;
  for (int r = 0; r < C4_ROWS; r++) if (c4board[r][3] == p) s += 3;                       // centre control
  for (int r = 0; r < C4_ROWS; r++) for (int c = 0; c <= C4_COLS-4; c++) s += c4Window(r, c, 0, 1, p);   // -
  for (int c = 0; c < C4_COLS; c++) for (int r = 0; r <= C4_ROWS-4; r++) s += c4Window(r, c, 1, 0, p);   // |
  for (int r = 0; r <= C4_ROWS-4; r++) for (int c = 0; c <= C4_COLS-4; c++) s += c4Window(r, c, 1, 1, p);  // down-right
  for (int r = 3; r < C4_ROWS; r++) for (int c = 0; c <= C4_COLS-4; c++) s += c4Window(r, c, -1, 1, p);  // up-right
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
    if (alpha >= beta) break;               // prune
  }
  return best;
}

int c4AIMove() {
  // Easy mode: now and then just play a random legal column (so it misses things).
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
  return pool[pn ? random(0, pn) : 0];       // random among equally-good moves -> variety
}

// --- Drawing ---
void c4DrawMenu() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 20); u8g2_display.print("NELJÄN SUORA");
  tft.drawFastHLine(0, 26, 128, COLOR_GRAY);

  const char* opt[3] = { "HELPPO", "VAIKEA", "2 PELAAJAA" };
  for (int i = 0; i < 3; i++) {
    int y = 50 + i * 24;
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
  u8g2_display.setCursor(2, 120); u8g2_display.print("ENTER = aloita");
}

void drawNeljanSuora() {
  if (c4phase == C4_MENU) { c4DrawMenu(); return; }

  tft.fillScreen(COLOR_BG);

  // Drop preview disc (in the current player's colour)
  if (!c4GameOver) {
    int cx = C4_X0 + c4cursorCol * C4_CELL + C4_CELL / 2;
    tft.fillCircle(cx, 7, 4, (c4turn == 1) ? C4_COL_P1 : C4_COL_P2);
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

  if (c4GameOver) {
    tft.fillRect(8, 44, 112, 56, COLOR_POPUP_BG);
    tft.drawRect(8, 44, 112, 56, COLOR_BORDER);

    // Line 1: result
    if (c4winner == 0) {
      u8g2_display.setForegroundColor(COLOR_TEXT);
      u8g2_display.setCursor(14, 62); u8g2_display.print("TASAPELI");
    } else if (c4mode == 0) {
      if (c4winner == 1) { u8g2_display.setForegroundColor(COLOR_GREEN);  u8g2_display.setCursor(14, 62); u8g2_display.print("VOITIT!"); }
      else               { u8g2_display.setForegroundColor(COLOR_YELLOW); u8g2_display.setCursor(14, 62); u8g2_display.print("KONE VOITTI"); }
    } else {
      u8g2_display.setForegroundColor(c4winner == 1 ? C4_COL_P1 : C4_COL_P2);
      u8g2_display.setCursor(14, 62); u8g2_display.print(c4winner == 1 ? "PUNAINEN" : "KELTAINEN");
    }

    // Line 2: context
    u8g2_display.setForegroundColor(COLOR_TEXT);
    if (c4mode == 0) { u8g2_display.setCursor(14, 79); u8g2_display.print("Voitot: "); u8g2_display.print(c4Wins); }
    else if (c4winner != 0) { u8g2_display.setCursor(14, 79); u8g2_display.print("VOITTI!"); }

    // Line 3: restart
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(14, 95); u8g2_display.print("ENTER = uusi");
  }
}

// --- Game flow ---
void initNeljanSuora() {
  Preferences p; p.begin("neljansuora", true);
  c4Wins = p.getInt("wins", 0);
  p.end();
}

void setupNeljanSuora() {        // launched from the main menu -> show the 1P/2P chooser
  c4phase = C4_MENU;
  c4menuSel = 0;
  drawNeljanSuora();
}

void c4StartGame() {
  for (int r = 0; r < C4_ROWS; r++)
    for (int c = 0; c < C4_COLS; c++) c4board[r][c] = 0;
  c4cursorCol = 3;
  c4turn = 1;
  c4GameOver = false;
  c4winner = 0;
  c4phase = C4_PLAY;
  drawNeljanSuora();
}

void c4SaveWin() {
  c4Wins++;
  Preferences p; p.begin("neljansuora", false);
  p.putInt("wins", c4Wins);
  p.end();
}

void c4DoMove(int col) {
  if (c4Drop(col, c4turn) < 0) return;                  // column full -> ignore
  if (c4HasFour(c4turn)) {
    c4GameOver = true; c4winner = c4turn;
    if (c4mode == 0 && c4turn == 1) c4SaveWin();        // count only your wins vs the device
    drawNeljanSuora(); return;
  }
  if (c4BoardFull()) { c4GameOver = true; c4winner = 0; drawNeljanSuora(); return; }

  if (c4mode == 0) {                                     // 1 player: device replies
    c4turn = 2;
    c4Drop(c4AIMove(), 2);
    if (c4HasFour(2)) { c4GameOver = true; c4winner = 2; drawNeljanSuora(); return; }
    if (c4BoardFull()) { c4GameOver = true; c4winner = 0; drawNeljanSuora(); return; }
    c4turn = 1;
  } else {                                               // 2 players: pass the turn
    c4turn = (c4turn == 1) ? 2 : 1;
  }
  drawNeljanSuora();
}

void handleNeljanSuoraInput(char c) {
  if (c4phase == C4_MENU) {
    if      (c == 0xB5) { if (c4menuSel > 0) c4menuSel--; drawNeljanSuora(); }   // Up
    else if (c == 0xB6) { if (c4menuSel < 2) c4menuSel++; drawNeljanSuora(); }   // Down
    else if (c == 0x0D || c == 0x0A) {
      if      (c4menuSel == 0) { c4mode = 0; c4SearchDepth = 2; c4BlunderPct = 35; }  // HELPPO
      else if (c4menuSel == 1) { c4mode = 0; c4SearchDepth = 6; c4BlunderPct = 0;  }  // VAIKEA
      else                     { c4mode = 1; }                                        // 2 PELAAJAA
      c4StartGame();
    }
    return;
  }

  if (c4GameOver) {
    if (c == 0x0D || c == 0x0A) c4StartGame();            // quick rematch, same mode
    return;
  }

  if (c == 0xB4) {                                         // Left
    c4cursorCol = (c4cursorCol > 0) ? c4cursorCol - 1 : C4_COLS - 1;
    drawNeljanSuora();
  } else if (c == 0xB7) {                                  // Right
    c4cursorCol = (c4cursorCol < C4_COLS - 1) ? c4cursorCol + 1 : 0;
    drawNeljanSuora();
  } else if (c == 0x0D || c == 0x0A || c == 0xB6) {        // Enter or Down -> drop
    c4DoMove(c4cursorCol);
  }
}

#endif
