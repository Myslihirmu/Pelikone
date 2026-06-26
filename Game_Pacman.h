#ifndef GAME_PACMAN_H
#define GAME_PACMAN_H
#include "Globals.h"

// ── Layout ──────────────────────────────────────────────────────────────────
// Original maze: 28 wide × 31 tall. We use 28×31 at 4px/tile = 112×124.
// Offset to center horizontally: (128-112)/2 = 8px
// HUD at bottom 4px isn't enough, so we put score in row 0 of maze (empty row).
// Maze pixel area: x=8..119, y=0..123, HUD y=124..127
#define PM_TILE    4
#define PM_COLS    28
#define PM_ROWS    31
#define PM_OX      8     // x offset to center maze
#define PM_OY      0     // y offset
#define PM_HUD_Y   124   // HUD strip y

// ── Colors ──────────────────────────────────────────────────────────────────
#define PM_COL_BG     0x0000
#define PM_COL_WALL   0x2A1F   // blue walls
#define PM_COL_DOT    0xFFF0   // pale yellow dots
#define PM_COL_PAC    0xFFE0   // yellow pac-man
#define PM_COL_POWER  0xFFF0   // power pellet
#define PM_COL_FRUIT  0xF800   // red fruit
#define PM_COL_FRIGHT 0x001F   // blue frightened ghost
#define PM_COL_FWARN  0xFFFF   // white flashing ghost
#define PM_COL_TEXT   0xFFFF
#define PM_COL_EYES   0xFFFF

// Ghost colors
#define PM_COL_BLINKY 0xF800   // red
#define PM_COL_PINKY  0xFCFF   // pink

// ── Timing ──────────────────────────────────────────────────────────────────
#define PM_FRAME_MS    60      // base frame time (~16 fps)
#define PM_FRIGHT_MS   6000    // frightened duration
#define PM_FWARN_MS    4000    // start flashing at this point
#define PM_FRUIT_MS    9000    // fruit visible duration

// ── Directions ──────────────────────────────────────────────────────────────
#define DIR_NONE  -1
#define DIR_UP     0
#define DIR_LEFT   1
#define DIR_DOWN   2
#define DIR_RIGHT  3

const int8_t pmDX[] = { 0, -1,  0,  1};
const int8_t pmDY[] = {-1,  0,  1,  0};

// ── Maze data ───────────────────────────────────────────────────────────────
// 0=empty, 1=wall, 2=dot, 3=power pellet, 4=ghost house door
// Classic Pac-Man maze (28×31)
// Each row stored as 28 nibbles → 14 bytes, but for clarity use full bytes.
// We store in PROGMEM to save RAM.

const uint8_t pmMazeInit[PM_ROWS][PM_COLS] PROGMEM = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 0
  {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1}, // 1
  {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1}, // 2
  {1,3,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,3,1}, // 3
  {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1}, // 4
  {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1}, // 5
  {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1}, // 6
  {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1}, // 7
  {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1}, // 8
  {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1}, // 9
  {0,0,0,0,0,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,0,0,0,0,0}, //10
  {0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0}, //11
  {0,0,0,0,0,1,2,1,1,0,1,1,1,4,4,1,1,1,0,1,1,2,1,0,0,0,0,0}, //12
  {1,1,1,1,1,1,2,1,1,0,1,0,0,0,0,0,0,1,0,1,1,2,1,1,1,1,1,1}, //13
  {0,0,0,0,0,0,2,0,0,0,1,0,0,0,0,0,0,1,0,0,0,2,0,0,0,0,0,0}, //14 tunnel
  {1,1,1,1,1,1,2,1,1,0,1,0,0,0,0,0,0,1,0,1,1,2,1,1,1,1,1,1}, //15
  {0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0}, //16
  {0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0}, //17
  {0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0}, //18
  {1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1}, //19
  {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1}, //20
  {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1}, //21
  {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1}, //22
  {1,3,2,2,1,1,2,2,2,2,2,2,2,0,0,2,2,2,2,2,2,2,1,1,2,2,3,1}, //23
  {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1}, //24
  {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1}, //25
  {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1}, //26
  {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1}, //27
  {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1}, //28
  {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1}, //29
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}  //30
};

// Working copy of maze (RAM)
uint8_t pmMaze[PM_ROWS][PM_COLS];

// ── Game state ──────────────────────────────────────────────────────────────
// Pac-Man
int   pmPX, pmPY;           // tile position
int   pmDir, pmNextDir;     // current and queued direction
int   pmLives;
int   pmScore, pmHiScore;
int   pmDotsLeft;
int   pmLevel;
bool  pmGameOver;
bool  pmLevelClear;
unsigned long pmLevelClearTime;
bool  pmDying;              // death animation
int   pmDeathFrame;
unsigned long pmDeathStart;

// Ghosts (2 ghosts: Blinky + Pinky)
#define PM_NUM_GHOSTS 2
int   pmGX[PM_NUM_GHOSTS], pmGY[PM_NUM_GHOSTS];   // tile positions
int   pmGDir[PM_NUM_GHOSTS];                        // current direction
bool  pmGHome[PM_NUM_GHOSTS];                       // in ghost house?
unsigned long pmGHomeExit[PM_NUM_GHOSTS];           // when to leave house

// Ghost modes
enum PMGhostMode { PM_CHASE, PM_SCATTER, PM_FRIGHTENED };
PMGhostMode pmGMode;
unsigned long pmFrightStart;
unsigned long pmModeStart;
int   pmModeIdx;             // index into scatter/chase cycle
bool  pmFrightFlash;         // flashing warning

// Scatter/chase cycle durations (ms) — simplified from original
// scatter 7s, chase 20s, scatter 7s, chase 20s, scatter 5s, chase 20s, scatter 5s, then chase forever
const unsigned long pmModeTimes[] = {7000, 20000, 7000, 20000, 5000, 20000, 5000, 0};
#define PM_NUM_MODES 8

// Fruit
bool  pmFruitActive;
unsigned long pmFruitStart;
int   pmFruitX, pmFruitY;   // tile pos (always 14, 17 in original)

// Timing
unsigned long pmLastFrame;
unsigned long pmPacMoveTime;   // pac moves every N ms
unsigned long pmGhostMoveTime; // ghosts move every N ms
unsigned long pmLastPacMove;
unsigned long pmLastGhostMove;
int   pmEatGhostScore;        // doubles for each ghost eaten in one power pellet

// ── Forward declarations ────────────────────────────────────────────────────
void pmDrawMaze();
void pmDrawTile(int col, int row);
void pmDrawPac();
void pmDrawGhost(int i);
void pmDrawHUD();
void pmDrawFruit();
void pmSpawnPac();
void pmSpawnGhosts();
void pmResetLevel();

// ═══════════════════════════════════════════════════════════════════════════
// Helpers
// ═══════════════════════════════════════════════════════════════════════════

bool pmIsWalkable(int col, int row) {
  if (row < 0 || row >= PM_ROWS) return false;
  // Tunnel wrap
  if (col < 0 || col >= PM_COLS) {
    if (row == 14) return true;  // tunnel row
    return false;
  }
  uint8_t t = pmMaze[row][col];
  return (t != 1);  // anything except wall
}

bool pmIsWalkableGhost(int col, int row, bool canUseDoor) {
  if (row < 0 || row >= PM_ROWS) return false;
  if (col < 0 || col >= PM_COLS) {
    if (row == 14) return true;
    return false;
  }
  uint8_t t = pmMaze[row][col];
  if (t == 1) return false;
  if (t == 4 && !canUseDoor) return false;
  return true;
}

int pmWrapCol(int col) {
  if (col < 0) return PM_COLS - 1;
  if (col >= PM_COLS) return 0;
  return col;
}

int pmDist2(int x1, int y1, int x2, int y2) {
  int dx = x1 - x2, dy = y1 - y2;
  return dx * dx + dy * dy;
}

// ═══════════════════════════════════════════════════════════════════════════
// Drawing
// ═══════════════════════════════════════════════════════════════════════════

void pmDrawTile(int col, int row) {
  int px = PM_OX + col * PM_TILE;
  int py = PM_OY + row * PM_TILE;
  uint8_t t = pmMaze[row][col];

  if (t == 1) {
    tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_WALL);
  } else if (t == 2) {
    tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_BG);
    tft.fillRect(px + 1, py + 1, 2, 2, PM_COL_DOT);   // small dot
  } else if (t == 3) {
    tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_BG);
    tft.fillCircle(px + 1, py + 1, 1, PM_COL_POWER);   // power pellet
  } else if (t == 4) {
    // Ghost house door — draw as dim line
    tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_BG);
    tft.drawFastHLine(px, py + 1, PM_TILE, 0xFBE0);    // pinkish door
  } else {
    tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_BG);
  }
}

void pmDrawMaze() {
  tft.fillScreen(PM_COL_BG);
  for (int r = 0; r < PM_ROWS; r++)
    for (int c = 0; c < PM_COLS; c++)
      pmDrawTile(c, r);
}

void pmDrawPac() {
  if (pmDying) return;  // handled separately
  int px = PM_OX + pmPX * PM_TILE;
  int py = PM_OY + pmPY * PM_TILE;
  tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_PAC);
  // Mouth: 1 dark pixel in movement direction
  int mx = px + 1, my = py + 1;
  if (pmDir == DIR_RIGHT) mx = px + 3;
  else if (pmDir == DIR_LEFT) mx = px;
  else if (pmDir == DIR_DOWN) my = py + 3;
  else if (pmDir == DIR_UP) my = py;
  tft.drawPixel(mx, my, PM_COL_BG);
}

void pmErasePac() {
  int px = PM_OX + pmPX * PM_TILE;
  int py = PM_OY + pmPY * PM_TILE;
  tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_BG);
}

void pmDrawGhost(int i) {
  int px = PM_OX + pmGX[i] * PM_TILE;
  int py = PM_OY + pmGY[i] * PM_TILE;
  uint16_t col;

  if (pmGMode == PM_FRIGHTENED && !pmGHome[i]) {
    // Flashing when time almost up
    if (pmFrightFlash && ((millis() / 200) & 1))
      col = PM_COL_FWARN;
    else
      col = PM_COL_FRIGHT;
  } else {
    col = (i == 0) ? PM_COL_BLINKY : PM_COL_PINKY;
  }

  // Ghost body: filled square with wavy bottom
  tft.fillRect(px, py, PM_TILE, PM_TILE - 1, col);
  // Wavy bottom: alternating pixels
  for (int x = 0; x < PM_TILE; x++)
    tft.drawPixel(px + x, py + PM_TILE - 1, (x & 1) ? col : PM_COL_BG);
  // Eyes: 2 white pixels
  tft.drawPixel(px + 1, py + 1, PM_COL_EYES);
  tft.drawPixel(px + 2, py + 1, PM_COL_EYES);
}

void pmEraseGhost(int i) {
  int px = PM_OX + pmGX[i] * PM_TILE;
  int py = PM_OY + pmGY[i] * PM_TILE;
  tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_BG);
  // Restore dot/power if present
  uint8_t t = pmMaze[pmGY[i]][pmGX[i]];
  if (t == 2 || t == 3) pmDrawTile(pmGX[i], pmGY[i]);
}

void pmDrawFruit() {
  if (!pmFruitActive) return;
  int px = PM_OX + pmFruitX * PM_TILE;
  int py = PM_OY + pmFruitY * PM_TILE;
  tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_BG);
  tft.fillCircle(px + 1, py + 2, 1, PM_COL_FRUIT);
  tft.drawPixel(px + 2, py, COLOR_GREEN);  // stem
}

void pmDrawHUD() {
  tft.fillRect(0, PM_HUD_Y, 128, 4, PM_COL_BG);

  // Score (tiny — just draw digits as pixels)
  // Use simple approach: small colored rectangles
  u8g2_display.setFont(u8g2_font_micro_tr);  // tiny 3px font
  u8g2_display.setForegroundColor(PM_COL_TEXT);
  u8g2_display.setCursor(1, 127);
  u8g2_display.print(pmScore);

  // Lives as yellow dots
  for (int i = 0; i < pmLives - 1; i++)
    tft.fillRect(80 + i * 6, PM_HUD_Y + 1, 3, 3, PM_COL_PAC);

  // Level indicator
  u8g2_display.setCursor(105, 127);
  u8g2_display.print("L");
  u8g2_display.print(pmLevel);

  // Restore main font for other games
  // (will be set back when leaving)
}

// ═══════════════════════════════════════════════════════════════════════════
// Ghost AI
// ═══════════════════════════════════════════════════════════════════════════

// Get target tile for ghost i
void pmGhostTarget(int i, int &tx, int &ty) {
  if (pmGMode == PM_SCATTER) {
    // Scatter: each ghost heads to its corner
    if (i == 0) { tx = PM_COLS - 3; ty = 0; }           // Blinky → top-right
    else        { tx = 2;            ty = 0; }           // Pinky → top-left
    return;
  }

  if (pmGMode == PM_FRIGHTENED) {
    // Random target
    tx = random(0, PM_COLS);
    ty = random(0, PM_ROWS);
    return;
  }

  // Chase mode
  if (i == 0) {
    // Blinky: target = Pac-Man's tile
    tx = pmPX; ty = pmPY;
  } else {
    // Pinky: target = 4 tiles ahead of Pac-Man
    tx = pmPX + pmDX[pmDir] * 4;
    ty = pmPY + pmDY[pmDir] * 4;
    // Clamp
    if (tx < 0) tx = 0;
    if (tx >= PM_COLS) tx = PM_COLS - 1;
    if (ty < 0) ty = 0;
    if (ty >= PM_ROWS) ty = PM_ROWS - 1;
  }
}

void pmMoveGhost(int i) {
  bool canUseDoor = pmGHome[i];

  // If in ghost house, move up to exit
  if (pmGHome[i]) {
    if (pmGY[i] > 11) {
      pmEraseGhost(i);
      pmGY[i]--;
      pmGDir[i] = DIR_UP;
      return;
    }
    // Exited house
    pmGHome[i] = false;
  }

  int tx, ty;
  pmGhostTarget(i, tx, ty);

  // Ghosts choose direction at each tile:
  // Cannot reverse, prefer direction closest to target
  int bestDir = -1;
  int bestDist = 999999;
  int curDir = pmGDir[i];
  int reverseDir = (curDir + 2) % 4;

  for (int d = 0; d < 4; d++) {
    if (d == reverseDir) continue;  // can't reverse

    int nx = pmGX[i] + pmDX[d];
    int ny = pmGY[i] + pmDY[d];

    // Tunnel wrap
    nx = pmWrapCol(nx);

    if (!pmIsWalkableGhost(nx, ny, canUseDoor)) continue;

    // Ghosts can't go up at certain tiles (original game restriction)
    // Simplified: allow all

    int dist = pmDist2(nx, ny, tx, ty);
    if (dist < bestDist) {
      bestDist = dist;
      bestDir = d;
    }
  }

  if (bestDir >= 0) {
    pmEraseGhost(i);
    pmGX[i] += pmDX[bestDir];
    pmGY[i] += pmDY[bestDir];
    pmGX[i] = pmWrapCol(pmGX[i]);
    pmGDir[i] = bestDir;
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// Game logic
// ═══════════════════════════════════════════════════════════════════════════

void pmCountDots() {
  pmDotsLeft = 0;
  for (int r = 0; r < PM_ROWS; r++)
    for (int c = 0; c < PM_COLS; c++)
      if (pmMaze[r][c] == 2 || pmMaze[r][c] == 3)
        pmDotsLeft++;
}

void pmSpawnPac() {
  pmPX = 14; pmPY = 23;  // original start position
  pmDir = DIR_LEFT;
  pmNextDir = DIR_LEFT;
}

void pmSpawnGhosts() {
  // Blinky starts outside house
  pmGX[0] = 14; pmGY[0] = 11;
  pmGDir[0] = DIR_LEFT;
  pmGHome[0] = false;

  // Pinky starts inside house
  pmGX[1] = 14; pmGY[1] = 14;
  pmGDir[1] = DIR_UP;
  pmGHome[1] = true;
  pmGHomeExit[1] = millis() + 4000;  // exits after 4 seconds

  pmGMode = PM_SCATTER;
  pmModeStart = millis();
  pmModeIdx = 0;
  pmEatGhostScore = 200;
}

void pmLoadMaze() {
  for (int r = 0; r < PM_ROWS; r++)
    for (int c = 0; c < PM_COLS; c++)
      pmMaze[r][c] = pgm_read_byte(&pmMazeInit[r][c]);
}

void pmResetLevel() {
  pmLoadMaze();
  pmCountDots();
  pmSpawnPac();
  pmSpawnGhosts();
  pmFruitActive = false;
  pmFruitX = 14; pmFruitY = 17;
  pmDying = false;
  pmLevelClear = false;

  // Speed scales with level
  pmPacMoveTime = max(80, 130 - pmLevel * 5);
  pmGhostMoveTime = max(90, 150 - pmLevel * 5);
  pmLastPacMove = millis();
  pmLastGhostMove = millis();

  pmDrawMaze();
  pmDrawPac();
  for (int i = 0; i < PM_NUM_GHOSTS; i++) pmDrawGhost(i);
  pmDrawHUD();
}

void pmStartFrightened() {
  pmGMode = PM_FRIGHTENED;
  pmFrightStart = millis();
  pmFrightFlash = false;
  pmEatGhostScore = 200;
  // Reverse all ghost directions
  for (int i = 0; i < PM_NUM_GHOSTS; i++) {
    if (!pmGHome[i])
      pmGDir[i] = (pmGDir[i] + 2) % 4;
  }
}

void pmMovePac() {
  // Try queued direction first
  int tryDir = pmNextDir;
  int nx = pmPX + pmDX[tryDir];
  int ny = pmPY + pmDY[tryDir];
  nx = pmWrapCol(nx);

  if (pmIsWalkable(nx, ny)) {
    pmDir = tryDir;
  } else {
    // Try current direction
    nx = pmPX + pmDX[pmDir];
    ny = pmPY + pmDY[pmDir];
    nx = pmWrapCol(nx);
    if (!pmIsWalkable(nx, ny)) return;  // stuck
  }

  nx = pmPX + pmDX[pmDir];
  ny = pmPY + pmDY[pmDir];
  nx = pmWrapCol(nx);

  // Erase old position
  pmErasePac();
  // Restore tile content at old position
  pmDrawTile(pmPX, pmPY);

  pmPX = nx;
  pmPY = ny;

  // Eat dot
  uint8_t t = pmMaze[pmPY][pmPX];
  if (t == 2) {
    pmMaze[pmPY][pmPX] = 0;
    pmScore += 10;
    pmDotsLeft--;
  } else if (t == 3) {
    pmMaze[pmPY][pmPX] = 0;
    pmScore += 50;
    pmDotsLeft--;
    pmStartFrightened();
  }

  // Fruit spawn: at 70 and 170 dots eaten
  int dotsEaten = 244 - pmDotsLeft;  // approximate total dots
  if (!pmFruitActive && (dotsEaten == 70 || dotsEaten == 170)) {
    pmFruitActive = true;
    pmFruitStart = millis();
  }

  // Eat fruit
  if (pmFruitActive && pmPX == pmFruitX && pmPY == pmFruitY) {
    pmScore += 100 * pmLevel;
    pmFruitActive = false;
    pmDrawTile(pmFruitX, pmFruitY);
  }

  // Check level clear
  if (pmDotsLeft <= 0) {
    pmLevelClear = true;
    pmLevelClearTime = millis();
    return;
  }

  pmDrawHUD();
}

void pmCheckCollisions() {
  for (int i = 0; i < PM_NUM_GHOSTS; i++) {
    if (pmGX[i] == pmPX && pmGY[i] == pmPY) {
      if (pmGMode == PM_FRIGHTENED && !pmGHome[i]) {
        // Eat ghost!
        pmScore += pmEatGhostScore;
        pmEatGhostScore *= 2;
        // Send ghost back to house
        pmEraseGhost(i);
        pmGX[i] = 14; pmGY[i] = 14;
        pmGHome[i] = true;
        pmGHomeExit[i] = millis() + 3000;
        pmDrawHUD();
      } else if (!pmGHome[i]) {
        // Pac-Man dies
        pmDying = true;
        pmDeathFrame = 0;
        pmDeathStart = millis();
      }
    }
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// Init / Setup / Draw / Update / Input
// ═══════════════════════════════════════════════════════════════════════════

void initPacman() {
  Preferences p;
  p.begin("pacman", true);
  pmHiScore = p.getInt("hiscore", 0);
  p.end();
}

void setupPacman() {
  pmScore = 0;
  pmLives = 3;
  pmLevel = 1;
  pmGameOver = false;
  pmLastFrame = millis();
  pmResetLevel();
}

void drawPacman() {
  if (pmGameOver) {
    tft.fillRect(14, 45, 100, 40, COLOR_POPUP_BG);
    tft.drawRect(14, 45, 100, 40, COLOR_BORDER);
    u8g2_display.setFont(u8g2_font_helvB12_te);
    u8g2_display.setForegroundColor(PM_COL_TEXT);
    u8g2_display.setCursor(20, 62);
    u8g2_display.print("GAME OVER");
    u8g2_display.setCursor(25, 80);
    u8g2_display.print("HI:"); u8g2_display.print(pmHiScore);
    return;
  }
  pmDrawMaze();
  for (int i = 0; i < PM_NUM_GHOSTS; i++) pmDrawGhost(i);
  if (pmFruitActive) pmDrawFruit();
  pmDrawPac();
  pmDrawHUD();
}

void updatePacman() {
  if (pmGameOver) return;

  unsigned long now = millis();

  // Level clear flash
  if (pmLevelClear) {
    if (now - pmLevelClearTime > 2000) {
      pmLevel++;
      pmResetLevel();
    }
    return;
  }

  // Death animation
  if (pmDying) {
    if (now - pmDeathStart > 1500) {
      pmDying = false;
      pmLives--;
      if (pmLives <= 0) {
        pmGameOver = true;
        if (pmScore > pmHiScore) {
          pmHiScore = pmScore;
          Preferences p;
          p.begin("pacman", false);
          p.putInt("hiscore", pmHiScore);
          p.end();
        }
        drawPacman();
        return;
      }
      // Respawn
      pmSpawnPac();
      pmSpawnGhosts();
      pmDrawMaze();
      for (int i = 0; i < PM_NUM_GHOSTS; i++) pmDrawGhost(i);
      pmDrawPac();
      pmDrawHUD();
    } else {
      // Simple death animation: shrinking pac
      int frame = (now - pmDeathStart) / 200;
      int px = PM_OX + pmPX * PM_TILE;
      int py = PM_OY + pmPY * PM_TILE;
      tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_BG);
      if (frame < 4) {
        int sz = PM_TILE - frame;
        if (sz > 0) tft.fillRect(px, py, sz, sz, PM_COL_PAC);
      }
    }
    return;
  }

  // Ghost mode cycling (scatter/chase)
  if (pmGMode != PM_FRIGHTENED) {
    if (pmModeIdx < PM_NUM_MODES - 1 && pmModeTimes[pmModeIdx] > 0) {
      if (now - pmModeStart >= pmModeTimes[pmModeIdx]) {
        pmModeIdx++;
        pmModeStart = now;
        pmGMode = (pmModeIdx & 1) ? PM_CHASE : PM_SCATTER;
        // Reverse ghosts on mode change
        for (int i = 0; i < PM_NUM_GHOSTS; i++)
          if (!pmGHome[i]) pmGDir[i] = (pmGDir[i] + 2) % 4;
      }
    }
  }

  // Frightened timer
  if (pmGMode == PM_FRIGHTENED) {
    unsigned long elapsed = now - pmFrightStart;
    pmFrightFlash = (elapsed >= PM_FWARN_MS);
    if (elapsed >= PM_FRIGHT_MS) {
      // End frightened
      pmGMode = (pmModeIdx & 1) ? PM_CHASE : PM_SCATTER;
    }
  }

  // Ghost house exit timer
  for (int i = 0; i < PM_NUM_GHOSTS; i++) {
    if (pmGHome[i] && now >= pmGHomeExit[i]) {
      // Start moving out
      // (handled in pmMoveGhost)
    }
  }

  // Fruit timeout
  if (pmFruitActive && now - pmFruitStart >= PM_FRUIT_MS) {
    pmFruitActive = false;
    pmDrawTile(pmFruitX, pmFruitY);
  }

  // Move Pac-Man
  if (now - pmLastPacMove >= pmPacMoveTime) {
    pmLastPacMove = now;
    pmMovePac();
    if (pmLevelClear) return;
    pmDrawPac();
    pmCheckCollisions();
    if (pmDying || pmGameOver) return;
  }

  // Move ghosts
  if (now - pmLastGhostMove >= pmGhostMoveTime) {
    pmLastGhostMove = now;
    for (int i = 0; i < PM_NUM_GHOSTS; i++) {
      if (pmGHome[i] && now < pmGHomeExit[i]) continue;
      pmMoveGhost(i);
      pmDrawGhost(i);
    }
    pmCheckCollisions();
    if (pmDying || pmGameOver) return;
  }

  // Redraw fruit on top
  if (pmFruitActive) pmDrawFruit();

  // Power pellet blink
  static unsigned long lastBlink = 0;
  if (now - lastBlink > 300) {
    lastBlink = now;
    for (int r = 0; r < PM_ROWS; r++)
      for (int c = 0; c < PM_COLS; c++)
        if (pmMaze[r][c] == 3) {
          int px = PM_OX + c * PM_TILE;
          int py = PM_OY + r * PM_TILE;
          if ((now / 300) & 1)
            tft.fillCircle(px + 1, py + 1, 1, PM_COL_POWER);
          else
            tft.fillRect(px, py, PM_TILE, PM_TILE, PM_COL_BG);
        }
  }
}

void handlePacmanInput(char c) {
  if (pmGameOver) {
    if (c == 0x0D || c == 0x0A) setupPacman();
    return;
  }
  if (pmDying) return;

  if      (c == 0xB5) pmNextDir = DIR_UP;
  else if (c == 0xB6) pmNextDir = DIR_DOWN;
  else if (c == 0xB4) pmNextDir = DIR_LEFT;
  else if (c == 0xB7) pmNextDir = DIR_RIGHT;
}

#endif
