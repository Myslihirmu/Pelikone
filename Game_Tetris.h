#ifndef GAME_TETRIS_H
#define GAME_TETRIS_H
#include "Globals.h"

// ── Layout ─────────────────────────────────────────────────────────────────
#define TET_COLS   10
#define TET_ROWS   20
#define TET_CELL    6    // px per cell
#define TET_BX      2    // board pixel X origin
#define TET_BY      8    // board pixel Y origin  (8 + 20*6 = 128, fills display)
#define TET_SX     66    // side-panel X start    (BX + COLS*CELL + 4 = 66)

// ── Piece colors  (I  O  T  S  Z  J  L) ───────────────────────────────────
const uint16_t TET_COLORS[7] = {
  0x07FF,  // I – cyan
  0xFFE0,  // O – yellow
  0xC81F,  // T – magenta
  0x07E0,  // S – green
  0xF800,  // Z – red
  0x001F,  // J – blue
  0xFD20,  // L – orange
};

#define TET_EMPTY 0x1082  // dark cell background

// ── Shapes  [piece][rotation][row] ─────────────────────────────────────────
// Row = 4-bit mask: bit3=col0 … bit0=col3
// Pieces use the left-most columns of the 4-wide shape grid.
// Rotation order is clockwise (0→1→2→3→0).
const uint8_t TET_SHAPES[7][4][4] PROGMEM = {
  // I   – 2 unique shapes, repeated
  {{ 0, 15,  0,  0}, { 2,  2,  2,  2}, { 0, 15,  0,  0}, { 2,  2,  2,  2}},
  // O   – same all rotations
  {{ 6,  6,  0,  0}, { 6,  6,  0,  0}, { 6,  6,  0,  0}, { 6,  6,  0,  0}},
  // T   – 4 unique  (.T./TTT  T./TT/T.  TTT/.T.  .T/TT/.T)
  {{ 4, 14,  0,  0}, { 4,  6,  4,  0}, {14,  4,  0,  0}, { 4, 12,  4,  0}},
  // S   – 2 unique  (.TT/TT.  T./TT/.T)
  {{ 6, 12,  0,  0}, { 8, 12,  4,  0}, { 6, 12,  0,  0}, { 8, 12,  4,  0}},
  // Z   – 2 unique  (TT./.TT  .T/TT/T.)
  {{12,  6,  0,  0}, { 4, 12,  8,  0}, {12,  6,  0,  0}, { 4, 12,  8,  0}},
  // J   – 4 unique  (T./TTT  .TT/.T./.T.  TTT/..T  .T./.T./TT.)
  {{ 8, 14,  0,  0}, { 6,  4,  4,  0}, {14,  2,  0,  0}, { 4,  4, 12,  0}},
  // L   – 4 unique  (..T/TTT  .T./.T./.TT  TTT/T..  TT./.T./.T.)
  {{ 2, 14,  0,  0}, { 4,  4,  6,  0}, {14,  8,  0,  0}, {12,  4,  4,  0}},
};

// Score per lines cleared × (level+1)
const int TET_SCORE_TABLE[5] = {0, 100, 300, 500, 800};

// ── State ──────────────────────────────────────────────────────────────────
uint8_t       tetBoard[TET_ROWS][TET_COLS]; // 0=empty, 1-7=locked piece type+1
int           tetType, tetRot, tetX, tetY;
int           tetNextType;
int           tetScore, tetHighScore, tetLevel, tetLines;
bool          tetGameOver;
unsigned long tetLastFall;

// ── Helpers ────────────────────────────────────────────────────────────────

// True if cell (r,c) of this piece/rotation is filled
inline bool tetCell(int type, int rot, int r, int c) {
  return (pgm_read_byte(&TET_SHAPES[type][rot][r]) >> (3 - c)) & 1;
}

// True if the piece fits at board position (x,y)
bool tetCanPlace(int type, int rot, int x, int y) {
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      if (!tetCell(type, rot, r, c)) continue;
      int br = y + r, bc = x + c;
      if (bc < 0 || bc >= TET_COLS || br >= TET_ROWS) return false;
      if (br >= 0 && tetBoard[br][bc] != 0)            return false;
    }
  }
  return true;
}

// ── Drawing ────────────────────────────────────────────────────────────────

void tetDrawCell(int row, int col, uint16_t color) {
  tft.fillRect(TET_BX + col * TET_CELL,
               TET_BY + row * TET_CELL,
               TET_CELL - 1, TET_CELL - 1, color);
}

// Paint the active piece's filled cells in `color` (pass TET_EMPTY to erase).
void tetDrawActive(uint16_t color) {
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      if (tetCell(tetType, tetRot, r, c)) {
        int br = tetY + r, bc = tetX + c;
        if (br >= 0 && br < TET_ROWS && bc >= 0 && bc < TET_COLS)
          tetDrawCell(br, bc, color);
      }
}

// Move the active piece from a previous (type,rot,x,y) to its current state:
// erase only the cells it vacated, then repaint it. Cells in the overlap are
// rewritten in the same color (invisible), so this is flicker-free — the same
// "erase only the sliver" idea Breakout uses for the paddle/ball.
void tetMoveActive(int oType, int oRot, int oX, int oY) {
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++) {
      if (!tetCell(oType, oRot, r, c)) continue;
      int br = oY + r, bc = oX + c;
      if (br < 0 || br >= TET_ROWS || bc < 0 || bc >= TET_COLS) continue;
      int nr = br - tetY, nc = bc - tetX;
      bool stillSet = (nr >= 0 && nr < 4 && nc >= 0 && nc < 4 && tetCell(tetType, tetRot, nr, nc));
      if (!stillSet && tetBoard[br][bc] == 0) tetDrawCell(br, bc, TET_EMPTY);
    }
  tetDrawActive(TET_COLORS[tetType]);
}

// Repaint just the locked-cell grid (used after line clears shift rows down).
// Same-color rewrites don't flicker, so only the cells that changed update.
void tetRedrawBoard() {
  for (int r = 0; r < TET_ROWS; r++)
    for (int c = 0; c < TET_COLS; c++)
      tetDrawCell(r, c, tetBoard[r][c] ? TET_COLORS[tetBoard[r][c] - 1] : TET_EMPTY);
}

void tetDrawSide() {
  tft.fillRect(TET_SX, 0, 128 - TET_SX, 128, COLOR_BG);

  if (tetGameOver) {
    u8g2_display.setForegroundColor(COLOR_GREEN);
    u8g2_display.setCursor(TET_SX,      28); u8g2_display.print("PELI");
    u8g2_display.setCursor(TET_SX,      46); u8g2_display.print("OHI!");
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(TET_SX,      68); u8g2_display.print("Pist:");
    u8g2_display.setCursor(TET_SX,      84); u8g2_display.print(tetScore);
    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(TET_SX,     110); u8g2_display.print("Enter");
    u8g2_display.setCursor(TET_SX,     126); u8g2_display.print("=uusi");
    return;
  }

  // "Next" label
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(TET_SX, 12);
  u8g2_display.print("SEUR.");

  // Next piece preview (24x24px area, 4x6px mini-cells)
  int nx = TET_SX + 3, ny = 16;
  tft.fillRect(nx, ny, 24, 24, TET_EMPTY);
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      if (tetCell(tetNextType, 0, r, c))
        tft.fillRect(nx + c * 6, ny + r * 6, 5, 5, TET_COLORS[tetNextType]);
    }
  }

  // Score
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(TET_SX, 56); u8g2_display.print("PIST.");
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(TET_SX, 70); u8g2_display.print(tetScore);

  // High score
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(TET_SX, 84); u8g2_display.print("HI:");
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(TET_SX, 98); u8g2_display.print(tetHighScore);

  // Level
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(TET_SX, 112); u8g2_display.print("LV:");
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(TET_SX, 126); u8g2_display.print(tetLevel + 1);
}

void drawTetrisBoard() {
  // Board background
  tft.fillRect(TET_BX, TET_BY, TET_COLS * TET_CELL, TET_ROWS * TET_CELL, TET_EMPTY);

  // Vertical border lines (full display height for clean look)
  tft.drawFastVLine(TET_BX - 1,                    0, 128, COLOR_BORDER);
  tft.drawFastVLine(TET_BX + TET_COLS * TET_CELL,  0, 128, COLOR_BORDER);

  // Locked cells
  for (int r = 0; r < TET_ROWS; r++)
    for (int c = 0; c < TET_COLS; c++)
      if (tetBoard[r][c])
        tetDrawCell(r, c, TET_COLORS[tetBoard[r][c] - 1]);

  // Active piece
  if (!tetGameOver) {
    for (int r = 0; r < 4; r++) {
      for (int c = 0; c < 4; c++) {
        if (tetCell(tetType, tetRot, r, c)) {
          int br = tetY + r, bc = tetX + c;
          if (br >= 0) tetDrawCell(br, bc, TET_COLORS[tetType]);
        }
      }
    }
  }

  tetDrawSide();
}

// ── Game logic ─────────────────────────────────────────────────────────────

// Lock the current piece, clear full lines, spawn the next piece
void tetLockAndSpawn() {
  // Stamp piece into board
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      if (!tetCell(tetType, tetRot, r, c)) continue;
      int br = tetY + r, bc = tetX + c;
      if (br >= 0 && br < TET_ROWS) tetBoard[br][bc] = tetType + 1;
    }
  }

  // Scan for full rows (bottom-to-top so shifting doesn't skip rows)
  int cleared = 0;
  for (int r = TET_ROWS - 1; r >= 0; r--) {
    bool full = true;
    for (int c = 0; c < TET_COLS; c++) if (!tetBoard[r][c]) { full = false; break; }
    if (full) {
      for (int rr = r; rr > 0; rr--)
        memcpy(&tetBoard[rr][0], &tetBoard[rr - 1][0], TET_COLS);
      memset(&tetBoard[0][0], 0, TET_COLS);
      cleared++; r++;  // re-check this row after rows shift down
    }
  }

  if (cleared > 0) {
    tetScore += TET_SCORE_TABLE[cleared] * (tetLevel + 1);
    tetLines += cleared;
    tetLevel  = tetLines / 10;
    if (tetScore > tetHighScore) {
      tetHighScore = tetScore;
      Preferences p; p.begin("tetris", false); p.putInt("hi", tetHighScore); p.end();
    }
    tetRedrawBoard();   // rows shifted down → repaint the locked grid
  }

  // Spawn next piece
  tetType     = tetNextType;
  tetNextType = random(0, 7);
  tetRot      = 0;
  tetX        = 3;
  tetY        = 0;

  if (!tetCanPlace(tetType, tetRot, tetX, tetY)) {
    tetGameOver = true;
    // Save hi score at game over in case no lines were cleared last piece
    if (tetScore > tetHighScore) {
      tetHighScore = tetScore;
      Preferences p; p.begin("tetris", false); p.putInt("hi", tetHighScore); p.end();
    }
    drawTetrisBoard();   // full redraw shows the game-over side panel
    return;
  }

  tetDrawActive(TET_COLORS[tetType]);  // paint the freshly spawned piece
  tetDrawSide();                       // next-piece / score / level changed
}

void initTetris() {
  Preferences p; p.begin("tetris", true); tetHighScore = p.getInt("hi", 0); p.end();
}

void setupTetris() {
  memset(tetBoard, 0, sizeof(tetBoard));
  tetScore = 0; tetLevel = 0; tetLines = 0;
  tetGameOver = false;
  tetNextType = random(0, 7);
  tetType     = random(0, 7);
  tetRot = 0; tetX = 3; tetY = 0;
  tetLastFall = millis();
  tft.fillScreen(COLOR_BG);
  drawTetrisBoard();
}

// ── Update (called every loop tick) ───────────────────────────────────────
void updateTetris() {
  if (tetGameOver) return;

  unsigned long interval = (unsigned long)max(100, 500 - tetLevel * 40);
  if (millis() - tetLastFall < interval) return;
  tetLastFall = millis();

  if (tetCanPlace(tetType, tetRot, tetX, tetY + 1)) {
    int oY = tetY; tetY++;
    tetMoveActive(tetType, tetRot, tetX, oY);
  } else {
    tetLockAndSpawn();   // handles its own lock/clear/spawn/side drawing
  }
}

// ── Input ──────────────────────────────────────────────────────────────────
void handleTetrisInput(char c) {
  if (tetGameOver) {
    if (c == 0x0D || c == 0x0A) setupTetris();
    return;
  }

  int oType = tetType, oRot = tetRot, oX = tetX, oY = tetY;
  bool moved = false;

  if (c == 0xB4) {       // ← left
    if (tetCanPlace(tetType, tetRot, tetX - 1, tetY)) { tetX--; moved = true; }
  }
  else if (c == 0xB7) {  // → right
    if (tetCanPlace(tetType, tetRot, tetX + 1, tetY)) { tetX++; moved = true; }
  }
  else if (c == 0xB6) {  // ↓ soft drop — move one step, reset fall timer
    if (tetCanPlace(tetType, tetRot, tetX, tetY + 1)) {
      tetY++; tetLastFall = millis(); moved = true;
    }
  }
  else if (c == 0xB5) {  // ↑ rotate CW, with simple wall-kick (try ±1)
    int nr = (tetRot + 1) % 4;
    if      (tetCanPlace(tetType, nr, tetX,     tetY)) { tetRot = nr;            moved = true; }
    else if (tetCanPlace(tetType, nr, tetX - 1, tetY)) { tetX--; tetRot = nr;   moved = true; }
    else if (tetCanPlace(tetType, nr, tetX + 1, tetY)) { tetX++; tetRot = nr;   moved = true; }
  }
  else if (c == ' ') {   // SPACE hard drop — slam piece to bottom, lock immediately
    while (tetCanPlace(tetType, tetRot, tetX, tetY + 1)) tetY++;
    tetMoveActive(oType, oRot, oX, oY);   // show the piece at its landing spot
    tetLockAndSpawn();
    tetLastFall = millis();
    return;
  }

  if (moved) tetMoveActive(oType, oRot, oX, oY);
}

#endif
