#ifndef GAME_BREAKOUT_H
#define GAME_BREAKOUT_H
#include "Globals.h"

// --- Layout ---
#define BRK_COLS        8
#define BRK_ROWS        5
#define BRK_CELL_W     16      // 8 * 16 = 128 px wide
#define BRK_BRICK_H     5
#define BRK_ROW_PITCH   7      // 5 px brick + 2 px gap
#define BRK_BRICK_TOP  16
#define BRK_HUD_H      13      // top strip for score/lives; also the ball's ceiling
#define BRK_PADDLE_W   24
#define BRK_PADDLE_H    3
#define BRK_PADDLE_Y  120
#define BRK_BALL        3
#define BRK_FRAME_DELAY 16     // ms between ball updates
#define BRK_LIVES       3

// Ball speed uses an accumulator: BRK_SPEED_UNIT == one 1px micro-step per frame,
// so values below it move the ball slower than 1px/frame (gentle start).
#define BRK_SPEED_UNIT  100
#define BRK_SPEED_START  65    // ~0.65 px/frame: a gentle but lively roll
#define BRK_SPEED_INC    12    // added each time the wall is cleared
#define BRK_SPEED_MAX   210

// Paddle glides smoothly while a direction is "active". Each key press refreshes
// the window, so rapid taps (or a held key, if the keypad repeats) flow together.
#define BRK_PADDLE_VEL    2    // px per frame while gliding
#define BRK_GLIDE_MS    170    // how long the paddle keeps moving after a press

// --- Colors (RGB565) ---
const uint16_t brkRowColors[BRK_ROWS] = { 0xF800, 0xFD20, 0xFFE0, 0x07E0, 0x07FF };
#define BRK_COL_PADDLE  0xFFFF
#define BRK_COL_BALL    0xFFFF

// --- State ---
bool brkAlive[BRK_ROWS][BRK_COLS];
int  brkBricksLeft;
int  brkPaddleX;
int  brkPaddleDir;             // -1 left, +1 right, 0 idle
unsigned long brkPaddleMoveUntil = 0;
int  brkBallX, brkBallY;
int  brkBallDX, brkBallDY;     // direction only, each is -1 or +1
int  brkSpeed;                 // ball speed in accumulator units (see BRK_SPEED_UNIT)
int  brkMoveAccum;             // fractional-pixel accumulator for the ball
int  brkLives;
int  brkScore, brkHiScore;
bool brkGameOver;
unsigned long brkLastUpdate = 0;

// --- Small helpers ---
int brkBrickX(int c) { return c * BRK_CELL_W; }
int brkBrickY(int r) { return BRK_BRICK_TOP + r * BRK_ROW_PITCH; }

void brkDrawBrick(int r, int c)  { tft.fillRect(brkBrickX(c), brkBrickY(r), BRK_CELL_W - 1, BRK_BRICK_H, brkRowColors[r]); }
void brkEraseBrick(int r, int c) { tft.fillRect(brkBrickX(c), brkBrickY(r), BRK_CELL_W - 1, BRK_BRICK_H, COLOR_BG); }
void brkDrawPaddle()  { tft.fillRect(brkPaddleX, BRK_PADDLE_Y, BRK_PADDLE_W, BRK_PADDLE_H, BRK_COL_PADDLE); }
void brkDrawBall()    { tft.fillRect(brkBallX, brkBallY, BRK_BALL, BRK_BALL, BRK_COL_BALL); }
void brkEraseBall()   { tft.fillRect(brkBallX, brkBallY, BRK_BALL, BRK_BALL, COLOR_BG); }

// Move the paddle to np (clamped), erasing only the strip it vacated — flicker-free.
void brkMovePaddleTo(int np) {
  if (np < 0) np = 0;
  if (np > 128 - BRK_PADDLE_W) np = 128 - BRK_PADDLE_W;
  if (np == brkPaddleX) return;
  if (np > brkPaddleX)
    tft.fillRect(brkPaddleX, BRK_PADDLE_Y, np - brkPaddleX, BRK_PADDLE_H, COLOR_BG);
  else
    tft.fillRect(np + BRK_PADDLE_W, BRK_PADDLE_Y, brkPaddleX - np, BRK_PADDLE_H, COLOR_BG);
  brkPaddleX = np;
  brkDrawPaddle();
}

void brkDrawHud() {
  tft.fillRect(0, 0, 128, BRK_HUD_H, COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(1, 11);
  u8g2_display.print(brkScore);
  for (int i = 0; i < brkLives; i++)
    tft.fillRect(124 - i * 7, 4, 5, 5, BRK_COL_PADDLE);
}

void brkDrawAllBricks() {
  for (int r = 0; r < BRK_ROWS; r++)
    for (int c = 0; c < BRK_COLS; c++)
      if (brkAlive[r][c]) brkDrawBrick(r, c);
}

void brkFillBricks() {
  brkBricksLeft = 0;
  for (int r = 0; r < BRK_ROWS; r++)
    for (int c = 0; c < BRK_COLS; c++) { brkAlive[r][c] = true; brkBricksLeft++; }
}

void brkResetBall() {
  brkBallX = brkPaddleX + BRK_PADDLE_W / 2 - BRK_BALL / 2;
  brkBallY = BRK_PADDLE_Y - BRK_BALL - 1;
  brkBallDY = -1;
  brkBallDX = random(0, 2) ? 1 : -1;
  brkMoveAccum = 0;
}

// Full-scene redraw — used by refreshDisplay() (e.g. after the battery popup)
// and on game over, so the screen is always self-consistent.
void drawBreakout() {
  tft.fillScreen(COLOR_BG);
  brkDrawHud();
  brkDrawAllBricks();
  brkDrawPaddle();
  if (!brkGameOver) brkDrawBall();
  if (brkGameOver) {
    tft.fillRect(14, 40, 100, 48, COLOR_POPUP_BG);
    tft.drawRect(14, 40, 100, 48, COLOR_BORDER);
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(24, 60); u8g2_display.print("PELI OHI!");
    u8g2_display.setCursor(24, 80); u8g2_display.print("HI: "); u8g2_display.print(brkHiScore);
  }
}

// Kills every alive brick overlapping the ball's current rect. Returns true if
// at least one was destroyed (caller flips the relevant velocity component).
bool brkBallHitsBrick() {
  bool hit = false;
  int c0 = brkBallX / BRK_CELL_W;
  int c1 = (brkBallX + BRK_BALL - 1) / BRK_CELL_W;
  if (c0 < 0) c0 = 0;
  if (c1 > BRK_COLS - 1) c1 = BRK_COLS - 1;
  for (int r = 0; r < BRK_ROWS; r++) {
    int by = brkBrickY(r);
    if (brkBallY + BRK_BALL - 1 < by || brkBallY > by + BRK_BRICK_H - 1) continue;
    for (int c = c0; c <= c1; c++) {
      if (brkAlive[r][c]) {
        brkAlive[r][c] = false;
        brkBricksLeft--;
        brkScore += (BRK_ROWS - r);   // higher rows are worth more
        brkEraseBrick(r, c);
        hit = true;
      }
    }
  }
  if (hit) brkDrawHud();
  return hit;
}

// Bounce off the paddle and steer based on where it was hit.
void brkCheckPaddle() {
  if (brkBallY + BRK_BALL - 1 >= BRK_PADDLE_Y &&
      brkBallY <= BRK_PADDLE_Y + BRK_PADDLE_H - 1 &&
      brkBallX + BRK_BALL - 1 >= brkPaddleX &&
      brkBallX <= brkPaddleX + BRK_PADDLE_W - 1) {
    brkBallY = BRK_PADDLE_Y - BRK_BALL;
    brkBallDY = -1;
    int ballCenter = brkBallX + BRK_BALL / 2;
    int padCenter = brkPaddleX + BRK_PADDLE_W / 2;
    brkBallDX = (ballCenter < padCenter) ? -1 : 1;
  }
}

// Move the ball one pixel per axis, resolving collisions per axis so it can
// never tunnel through a brick or wall regardless of speed.
void brkMoveBallOnce() {
  // X axis
  brkBallX += brkBallDX;
  if (brkBallX < 0) { brkBallX = 0; brkBallDX = 1; }
  else if (brkBallX > 127 - (BRK_BALL - 1)) { brkBallX = 127 - (BRK_BALL - 1); brkBallDX = -1; }
  if (brkBallHitsBrick()) { brkBallX -= brkBallDX; brkBallDX = -brkBallDX; }

  // Y axis
  brkBallY += brkBallDY;
  if (brkBallY < BRK_HUD_H) { brkBallY = BRK_HUD_H; brkBallDY = 1; }
  if (brkBallHitsBrick()) { brkBallY -= brkBallDY; brkBallDY = -brkBallDY; }
  if (brkBallDY > 0) brkCheckPaddle();
}

// --- Init / Setup ---
void initBreakout() {
  Preferences p;
  p.begin("breakout", true);
  brkHiScore = p.getInt("hiscore", 0);
  p.end();
}

void setupBreakout() {
  brkScore = 0;
  brkLives = BRK_LIVES;
  brkSpeed = BRK_SPEED_START;
  brkMoveAccum = 0;
  brkPaddleDir = 0;
  brkGameOver = false;
  brkPaddleX = (128 - BRK_PADDLE_W) / 2;
  brkFillBricks();
  brkResetBall();
  brkLastUpdate = millis();
  drawBreakout();
}

// --- Update (main game loop) ---
void updateBreakout() {
  if (brkGameOver) return;
  if (millis() - brkLastUpdate < BRK_FRAME_DELAY) return;
  brkLastUpdate = millis();

  // Paddle: glide while the last press is still "active"
  if (brkPaddleDir != 0) {
    if (millis() < brkPaddleMoveUntil) brkMovePaddleTo(brkPaddleX + brkPaddleDir * BRK_PADDLE_VEL);
    else brkPaddleDir = 0;
  }

  // Ball: advance by whole pixels; fractional speed via the accumulator
  brkMoveAccum += brkSpeed;
  if (brkMoveAccum < BRK_SPEED_UNIT) return;   // not enough built up to move yet

  brkEraseBall();
  bool lost = false;
  while (brkMoveAccum >= BRK_SPEED_UNIT) {
    brkMoveAccum -= BRK_SPEED_UNIT;
    brkMoveBallOnce();
    if (brkBallY > 124) { lost = true; break; }   // fell past the paddle
  }

  if (lost) {
    brkLives--;
    brkDrawHud();
    if (brkLives <= 0) {
      brkGameOver = true;
      if (brkScore > brkHiScore) {
        brkHiScore = brkScore;
        Preferences p;
        p.begin("breakout", false);
        p.putInt("hiscore", brkHiScore);
        p.end();
      }
      drawBreakout();
      return;
    }
    delay(600);
    brkResetBall();
  }

  brkDrawBall();

  if (brkBricksLeft <= 0) {   // board cleared -> next, faster level
    brkSpeed += BRK_SPEED_INC;
    if (brkSpeed > BRK_SPEED_MAX) brkSpeed = BRK_SPEED_MAX;
    delay(600);
    brkEraseBall();
    brkFillBricks();
    brkDrawAllBricks();
    brkResetBall();
    brkDrawHud();
    brkDrawBall();
  }
}

// --- Input ---
// A key press sets the glide direction and (re)opens the movement window; the
// actual smooth motion happens in updateBreakout().
void handleBreakoutInput(char c) {
  if (brkGameOver) {
    if (c == 0x0D || c == 0x0A) setupBreakout();
    return;
  }
  if (c == 0xB4) {            // Left
    brkPaddleDir = -1;
    brkPaddleMoveUntil = millis() + BRK_GLIDE_MS;
  } else if (c == 0xB7) {     // Right
    brkPaddleDir = 1;
    brkPaddleMoveUntil = millis() + BRK_GLIDE_MS;
  }
}

#endif
