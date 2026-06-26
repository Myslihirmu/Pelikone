#ifndef GAME_RALLIF1_H
#define GAME_RALLIF1_H
#include "Globals.h"

// --- Layout ---
#define RF1_ROAD_LEFT    13
#define RF1_ROAD_RIGHT  114
#define RF1_LANE_W       34
#define RF1_CAR_W        16
#define RF1_CAR_H        24
#define RF1_PLAYER_Y     96
#define RF1_MAX_ENEMIES   3
#define RF1_FRAME_DELAY  30
#define RF1_DASH_LEN      8
#define RF1_DASH_GAP      8

// --- Difficulty ---
#define RF1_INIT_SPEED    2
#define RF1_MAX_SPEED     5
#define RF1_SPEED_INC    10   // score interval for speed bump
#define RF1_2CAR_SCORE   15
#define RF1_3CAR_SCORE   30

// --- Colors (RGB565) ---
#define RF1_COL_ROAD     0x4208
#define RF1_COL_GRASS    0x03E0
#define RF1_COL_EDGE     0xFFFF
#define RF1_COL_DASH     0xFFFF
#define RF1_COL_PLAYER   0xF800
#define RF1_COL_WINDSH   0x001F
#define RF1_COL_WHEEL    0x0000

// Note: enemy blue is 0x000F (darker), NOT 0x001F — that's the windshield color,
// and a 0x001F car body would hide its own windshield.
const uint16_t rf1EnemyColors[] = { 0x07E0, 0x000F, 0xFFE0, 0xF81F, 0x07FF, 0xFBE0 };
#define RF1_NUM_COLORS 6

// --- State ---
struct RF1Enemy {
  int x, y, lane;
  uint16_t color;
  bool active;
};

int rf1PlayerLane = 1;
int rf1PlayerX;
int rf1PrevPlayerX;
RF1Enemy rf1Enemies[RF1_MAX_ENEMIES];
int rf1DashOffset = 0;
int rf1Score = 0;
int rf1HiScore = 0;
bool rf1GameOver = false;
int rf1Speed = RF1_INIT_SPEED;
int rf1MaxActive = 1;
unsigned long rf1LastUpdate = 0;

// --- Helpers ---
int rf1LaneX(int lane) {
  return RF1_ROAD_LEFT + lane * RF1_LANE_W + (RF1_LANE_W - RF1_CAR_W) / 2;
}

void rf1EraseCarArea(int x, int y) {
  tft.fillRect(x - 1, y, RF1_CAR_W + 2, RF1_CAR_H, RF1_COL_ROAD);
}

void rf1DrawPlayerCar(int x, int y) {
  // Body
  tft.fillRect(x, y, RF1_CAR_W, RF1_CAR_H, RF1_COL_PLAYER);
  // Windshield (top — facing up)
  tft.fillRect(x + 3, y + 4, 10, 4, RF1_COL_WINDSH);
  // Wheels
  tft.fillRect(x - 1, y + 2, 3, 5, RF1_COL_WHEEL);
  tft.fillRect(x + RF1_CAR_W - 2, y + 2, 3, 5, RF1_COL_WHEEL);
  tft.fillRect(x - 1, y + 17, 3, 5, RF1_COL_WHEEL);
  tft.fillRect(x + RF1_CAR_W - 2, y + 17, 3, 5, RF1_COL_WHEEL);
}

void rf1DrawEnemyCar(int x, int y, uint16_t col) {
  // Body
  tft.fillRect(x, y, RF1_CAR_W, RF1_CAR_H, col);
  // Windshield (bottom — facing down)
  tft.fillRect(x + 3, y + 16, 10, 4, RF1_COL_WINDSH);
  // Wheels
  tft.fillRect(x - 1, y + 2, 3, 5, RF1_COL_WHEEL);
  tft.fillRect(x + RF1_CAR_W - 2, y + 2, 3, 5, RF1_COL_WHEEL);
  tft.fillRect(x - 1, y + 17, 3, 5, RF1_COL_WHEEL);
  tft.fillRect(x + RF1_CAR_W - 2, y + 17, 3, 5, RF1_COL_WHEEL);
}

// --- Road drawing ---
void rf1DrawEdgeLines() {
  tft.drawFastVLine(RF1_ROAD_LEFT, 0, 128, RF1_COL_EDGE);
  tft.drawFastVLine(RF1_ROAD_LEFT + 1, 0, 128, RF1_COL_EDGE);
  tft.drawFastVLine(RF1_ROAD_RIGHT - 1, 0, 128, RF1_COL_EDGE);
  tft.drawFastVLine(RF1_ROAD_RIGHT, 0, 128, RF1_COL_EDGE);
}

void rf1DrawDashes() {
  int step = RF1_DASH_LEN + RF1_DASH_GAP;
  for (int lx = 0; lx < 2; lx++) {
    int x = RF1_ROAD_LEFT + (lx + 1) * RF1_LANE_W;
    // Erase the full column strip
    tft.drawFastVLine(x, 0, 128, RF1_COL_ROAD);
    tft.drawFastVLine(x + 1, 0, 128, RF1_COL_ROAD);
    // Draw dashes
    for (int y = -step + rf1DashOffset; y < 128; y += step) {
      int dy = y;
      int dh = RF1_DASH_LEN;
      // Clip to screen
      if (dy < 0) { dh += dy; dy = 0; }
      if (dy + dh > 128) dh = 128 - dy;
      if (dh > 0) {
        tft.fillRect(x, dy, 2, dh, RF1_COL_DASH);
      }
    }
  }
}

void rf1DrawRoad() {
  // Grass margins
  tft.fillRect(0, 0, RF1_ROAD_LEFT, 128, RF1_COL_GRASS);
  tft.fillRect(RF1_ROAD_RIGHT + 1, 0, 128 - RF1_ROAD_RIGHT, 128, RF1_COL_GRASS);
  // Road surface
  tft.fillRect(RF1_ROAD_LEFT, 0, RF1_ROAD_RIGHT - RF1_ROAD_LEFT + 1, 128, RF1_COL_ROAD);
  // Edge lines
  rf1DrawEdgeLines();
  // Lane dashes
  rf1DrawDashes();
}

// --- Init / Setup ---
void initRalliF1() {
  Preferences p;
  p.begin("rallif1", true);
  rf1HiScore = p.getInt("hiscore", 0);
  p.end();
}

void setupRalliF1() {
  rf1PlayerLane = 1;
  rf1PlayerX = rf1LaneX(1);
  rf1PrevPlayerX = rf1PlayerX;
  rf1Score = 0;
  rf1GameOver = false;
  rf1Speed = RF1_INIT_SPEED;
  rf1MaxActive = 1;
  rf1DashOffset = 0;
  rf1LastUpdate = millis();

  for (int i = 0; i < RF1_MAX_ENEMIES; i++) rf1Enemies[i].active = false;

  rf1DrawRoad();
  rf1DrawPlayerCar(rf1PlayerX, RF1_PLAYER_Y);
}

// --- Difficulty update ---
void rf1UpdateDifficulty() {
  rf1Speed = RF1_INIT_SPEED + rf1Score / RF1_SPEED_INC;
  if (rf1Speed > RF1_MAX_SPEED) rf1Speed = RF1_MAX_SPEED;
  rf1MaxActive = 1;
  if (rf1Score >= RF1_2CAR_SCORE) rf1MaxActive = 2;
  if (rf1Score >= RF1_3CAR_SCORE) rf1MaxActive = 3;
}

// --- Spawn ---
void rf1SpawnEnemy() {
  // Count active
  int active = 0;
  for (int i = 0; i < RF1_MAX_ENEMIES; i++) {
    if (rf1Enemies[i].active) {
      active++;
      // Don't spawn if any enemy is still near top
      if (rf1Enemies[i].y < RF1_CAR_H + 8) return;
    }
  }
  if (active >= rf1MaxActive) return;

  // Find free slot
  int slot = -1;
  for (int i = 0; i < RF1_MAX_ENEMIES; i++) {
    if (!rf1Enemies[i].active) { slot = i; break; }
  }
  if (slot < 0) return;

  // Pick lane not occupied by another enemy near top
  int lane = random(0, 3);
  for (int attempt = 0; attempt < 5; attempt++) {
    bool ok = true;
    for (int i = 0; i < RF1_MAX_ENEMIES; i++) {
      if (rf1Enemies[i].active && rf1Enemies[i].lane == lane && rf1Enemies[i].y < 60) {
        ok = false; break;
      }
    }
    if (ok) break;
    lane = random(0, 3);
  }

  rf1Enemies[slot].lane = lane;
  rf1Enemies[slot].x = rf1LaneX(lane);
  rf1Enemies[slot].y = -RF1_CAR_H;
  rf1Enemies[slot].color = rf1EnemyColors[random(0, RF1_NUM_COLORS)];
  rf1Enemies[slot].active = true;
}

// --- Collision ---
bool rf1CheckCollision(int ex, int ey) {
  // Simple rectangle overlap
  int px = rf1PlayerX - 1;
  int pw = RF1_CAR_W + 2;
  int py = RF1_PLAYER_Y;
  int ph = RF1_CAR_H;
  int exx = ex - 1;
  int ew = RF1_CAR_W + 2;
  int eh = RF1_CAR_H;

  return !(exx + ew <= px || exx >= px + pw || ey + eh <= py || ey >= py + ph);
}

// --- Draw (score + game over) ---
void drawRalliF1() {
  if (rf1GameOver) {
    tft.fillRect(14, 40, 100, 48, COLOR_POPUP_BG);
    tft.drawRect(14, 40, 100, 48, COLOR_BORDER);
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(28, 60);
    u8g2_display.print("KOLARI!");
    u8g2_display.setCursor(28, 80);
    u8g2_display.print("HI: "); u8g2_display.print(rf1HiScore);
    return;
  }

  // Score on grass (top-left)
  tft.fillRect(0, 0, RF1_ROAD_LEFT - 1, 16, RF1_COL_GRASS);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(1, 13);
  u8g2_display.print(rf1Score);
}

// --- Update (main game loop) ---
void updateRalliF1() {
  if (rf1GameOver) return;
  if (millis() - rf1LastUpdate < RF1_FRAME_DELAY) return;
  rf1LastUpdate = millis();

  // Animate dashes
  rf1DashOffset = (rf1DashOffset + rf1Speed) % (RF1_DASH_LEN + RF1_DASH_GAP);
  rf1DrawDashes();
  // Redraw edge lines in case dashes clipped them
  rf1DrawEdgeLines();

  // Move enemies
  for (int i = 0; i < RF1_MAX_ENEMIES; i++) {
    if (!rf1Enemies[i].active) continue;

    // Erase old position (only if on screen)
    if (rf1Enemies[i].y >= -RF1_CAR_H && rf1Enemies[i].y < 128) {
      int ey = rf1Enemies[i].y;
      int eh = RF1_CAR_H;
      // Clip erase area to screen
      if (ey < 0) { eh += ey; ey = 0; }
      tft.fillRect(rf1Enemies[i].x - 1, ey, RF1_CAR_W + 2, eh, RF1_COL_ROAD);
    }

    rf1Enemies[i].y += rf1Speed;

    // Passed bottom of screen
    if (rf1Enemies[i].y > 128) {
      rf1Enemies[i].active = false;
      rf1Score++;
      rf1UpdateDifficulty();
      continue;
    }

    // Collision check
    if (rf1CheckCollision(rf1Enemies[i].x, rf1Enemies[i].y)) {
      rf1GameOver = true;
      if (rf1Score > rf1HiScore) {
        rf1HiScore = rf1Score;
        Preferences p;
        p.begin("rallif1", false);
        p.putInt("hiscore", rf1HiScore);
        p.end();
      }
      drawRalliF1();
      return;
    }

    // Draw at new position (clip to screen)
    if (rf1Enemies[i].y + RF1_CAR_H > 0 && rf1Enemies[i].y < 128) {
      rf1DrawEnemyCar(rf1Enemies[i].x, rf1Enemies[i].y, rf1Enemies[i].color);
    }
  }

  // Spawn
  rf1SpawnEnemy();

  // Redraw player car (enemies may have erased part of it during movement)
  rf1DrawPlayerCar(rf1PlayerX, RF1_PLAYER_Y);

  drawRalliF1();
}

// --- Input ---
void handleRalliF1Input(char c) {
  if (rf1GameOver) {
    if (c == 0x0D || c == 0x0A) setupRalliF1();
    return;
  }

  int oldLane = rf1PlayerLane;
  if (c == 0xB4 && rf1PlayerLane > 0) rf1PlayerLane--;       // Left
  else if (c == 0xB7 && rf1PlayerLane < 2) rf1PlayerLane++;   // Right

  if (rf1PlayerLane != oldLane) {
    rf1EraseCarArea(rf1PlayerX, RF1_PLAYER_Y);
    rf1PlayerX = rf1LaneX(rf1PlayerLane);
    rf1DrawPlayerCar(rf1PlayerX, RF1_PLAYER_Y);
    // Redraw dashes + edges that may have been erased
    rf1DrawDashes();
    rf1DrawEdgeLines();
  }
}

#endif
