#ifndef GAME_ALIENSHOOT_H
#define GAME_ALIENSHOOT_H
#include "Globals.h"

// ── Layout ──────────────────────────────────────────────────────────────────
#define AS_HUD_H      14
#define AS_GROUND_Y   120
#define AS_GROUND_H   8

// ── Colors ──────────────────────────────────────────────────────────────────
#define AS_SKY_COL    0x1928   // dark night sky
#define AS_GROUND_COL 0x3186   // dark gray ground
#define AS_CROSS_COL  0xFFFF   // white crosshair
#define AS_CROSS_CD   0xF800   // red crosshair (cooldown)
#define AS_STAR_COL   0xC618   // dim white stars

// ── Settings ────────────────────────────────────────────────────────────────
#define AS_CROSS_STEP  4
#define AS_CROSS_SIZE  3       // half-length of crosshair arm (7px total)
#define AS_HUNT_MS     12000   // starting time to find each alien (ms)
#define AS_HUNT_MIN    6000    // minimum hunt time at high scores
#define AS_MISS_MS     1500    // cooldown after missed shot
#define AS_EXP_FRAMES  4
#define AS_EXP_MS      80
#define AS_FRAME_MS    30

// ── Building palette (7 distinct colors) ────────────────────────────────────
#define AS_NUM_COLORS  7
const uint16_t asPalette[AS_NUM_COLORS] = {
  0xF800,  // red
  0x07E0,  // green
  0x03BF,  // teal
  0xFFE0,  // yellow
  0xF81F,  // magenta
  0x07FF,  // cyan
  0xFD20   // orange
};

// ── Alien bitmap 8×10 ──────────────────────────────────────────────────────
#define AS_AL_W 8
#define AS_AL_H 10
const uint8_t asAlBmp[AS_AL_H] = {
  0b01000010,  // antennae tips
  0b01000010,  // antennae
  0b00111100,  // head top
  0b01111110,  // head
  0b01111110,  // head
  0b00111100,  // neck
  0b00111100,  // body
  0b01111110,  // arms out
  0b00111100,  // waist
  0b01100110   // legs
};

// ── Building data ───────────────────────────────────────────────────────────
#define AS_MAX_BLD 7
int  asBX[AS_MAX_BLD], asBW[AS_MAX_BLD], asBH[AS_MAX_BLD];
uint16_t asBC[AS_MAX_BLD];
int  asNBld;

// ── Stars ───────────────────────────────────────────────────────────────────
#define AS_NUM_STARS 15
int asStarX[AS_NUM_STARS], asStarY[AS_NUM_STARS];

// ── Alien state ─────────────────────────────────────────────────────────────
int  asAX, asAY;       // top-left of alien
int  asABld;           // which building index
bool asATop;           // true = rooftop, false = side

// ── Crosshair ───────────────────────────────────────────────────────────────
int  asCX, asCY;       // current position
int  asPX, asPY;       // previous position (for erase)

// ── Game state ──────────────────────────────────────────────────────────────
int  asScore, asHi;
bool asMissCD;         // miss cooldown active (can't shoot)
unsigned long asMissStart;
unsigned long asHuntStart; // when current alien spawned
bool asGameOver;
bool asExpActive;      // explosion active
int  asExpF;           // explosion frame
unsigned long asExpStart;
int  asExpCX, asExpCY; // explosion center
unsigned long asLastUpd;

// ═══════════════════════════════════════════════════════════════════════════
// Helpers
// ═══════════════════════════════════════════════════════════════════════════

// Return the base background color at (x,y) — sky, building, or ground
uint16_t asGetBg(int x, int y) {
  if (y >= AS_GROUND_Y) return AS_GROUND_COL;
  if (y < AS_HUD_H)    return COLOR_BG;
  for (int i = 0; i < asNBld; i++) {
    int top = AS_GROUND_Y - asBH[i];
    if (x >= asBX[i] && x < asBX[i] + asBW[i] && y >= top && y < AS_GROUND_Y)
      return asBC[i];
  }
  return AS_SKY_COL;
}

// Check if (x,y) is on an alien pixel
bool asHitAl(int x, int y) {
  int lx = x - asAX, ly = y - asAY;
  if (lx < 0 || lx >= AS_AL_W || ly < 0 || ly >= AS_AL_H) return false;
  return (asAlBmp[ly] & (0x80 >> lx)) != 0;
}

// Restore a pixel to its proper background (alien-aware)
void asRestorePx(int x, int y) {
  if (x < 0 || x > 127 || y < 0 || y > 127) return;
  if (asHitAl(x, y)) {
    // Eyes are black, rest is building color
    bool isEye = (y == asAY + 3) && (x == asAX + 2 || x == asAX + 5);
    tft.drawPixel(x, y, isEye ? 0x0000 : asBC[asABld]);
  } else
    tft.drawPixel(x, y, asGetBg(x, y));
}

// ═══════════════════════════════════════════════════════════════════════════
// Drawing
// ═══════════════════════════════════════════════════════════════════════════

void asDrawAlien() {
  uint16_t col = asBC[asABld];
  for (int r = 0; r < AS_AL_H; r++)
    for (int c = 0; c < AS_AL_W; c++)
      if (asAlBmp[r] & (0x80 >> c))
        tft.drawPixel(asAX + c, asAY + r, col);
  // Black eyes (row 3, cols 2 and 5)
  tft.drawPixel(asAX + 2, asAY + 3, 0x0000);
  tft.drawPixel(asAX + 5, asAY + 3, 0x0000);
}

void asEraseAlien() {
  for (int r = 0; r < AS_AL_H; r++)
    for (int c = 0; c < AS_AL_W; c++)
      if (asAlBmp[r] & (0x80 >> c)) {
        int px = asAX + c, py = asAY + r;
        tft.drawPixel(px, py, asGetBg(px, py));
      }
}

void asDrawCross() {
  uint16_t col = asMissCD ? AS_CROSS_CD : AS_CROSS_COL;
  for (int i = -AS_CROSS_SIZE; i <= AS_CROSS_SIZE; i++) {
    tft.drawPixel(asCX + i, asCY, col);
    tft.drawPixel(asCX, asCY + i, col);
  }
}

void asEraseCross() {
  for (int i = -AS_CROSS_SIZE; i <= AS_CROSS_SIZE; i++) {
    asRestorePx(asPX + i, asPY);
    asRestorePx(asPX, asPY + i);
  }
}

// Current hunt time limit based on score
int asHuntTime() {
  int t = AS_HUNT_MS - asScore * 200;
  return max((int)AS_HUNT_MIN, t);
}

void asDrawHUD() {
  tft.fillRect(0, 0, 128, AS_HUD_H, COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(2, 12);
  u8g2_display.print(asScore);

  // High score right-aligned
  u8g2_display.setCursor(75, 12);
  u8g2_display.print("HI:");
  u8g2_display.print(asHi);

  // Hunt timer bar (green → yellow → red as time runs out)
  if (!asExpActive && !asGameOver) {
    int total = asHuntTime();
    int elapsed = (int)(millis() - asHuntStart);
    int rem = total - elapsed;
    if (rem < 0) rem = 0;
    int barW = map(rem, 0, total, 0, 50);
    int pct = (rem * 100) / total;
    uint16_t barCol = (pct > 50) ? COLOR_GREEN : (pct > 25) ? COLOR_YELLOW : 0xF800;
    tft.fillRect(24, 4, barW, 5, barCol);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// Scene generation
// ═══════════════════════════════════════════════════════════════════════════

void asGenBuildings() {
  asNBld = 5 + random(0, 3);          // 5–7
  int gap = 2;
  int remain = 128 - gap * (asNBld - 1);

  int x = 0;
  for (int i = 0; i < asNBld; i++) {
    asBX[i] = x;
    if (i == asNBld - 1) {
      asBW[i] = 128 - x;              // fill to edge
    } else {
      int avg = remain / (asNBld - i);
      asBW[i] = avg + random(-3, 4);
      if (asBW[i] < 12) asBW[i] = 12;
      remain -= asBW[i];
    }
    asBH[i] = 35 + random(0, 45);     // 35–79 px tall
    asBC[i] = asPalette[i % AS_NUM_COLORS];
    x += asBW[i] + gap;
  }

  // Random stars
  for (int i = 0; i < AS_NUM_STARS; i++) {
    asStarX[i] = random(0, 128);
    asStarY[i] = AS_HUD_H + 2 + random(0, 30); // upper sky area
  }
}

void asDrawScene() {
  // Sky
  tft.fillRect(0, AS_HUD_H, 128, AS_GROUND_Y - AS_HUD_H, AS_SKY_COL);

  // Stars
  for (int i = 0; i < AS_NUM_STARS; i++)
    tft.drawPixel(asStarX[i], asStarY[i], AS_STAR_COL);

  // Ground
  tft.fillRect(0, AS_GROUND_Y, 128, AS_GROUND_H, AS_GROUND_COL);

  // Buildings
  for (int i = 0; i < asNBld; i++) {
    int top = AS_GROUND_Y - asBH[i];
    tft.fillRect(asBX[i], top, asBW[i], asBH[i], asBC[i]);

    // Roof edge — 1px darker line
    uint16_t roofCol = (asBC[i] >> 1) & 0x7BEF;
    tft.drawFastHLine(asBX[i], top, asBW[i], roofCol);

    // Windows — small dark rectangles
    uint16_t winCol = roofCol;
    for (int wy = top + 5; wy < AS_GROUND_Y - 5; wy += 9)
      for (int wx = asBX[i] + 3; wx + 3 <= asBX[i] + asBW[i] - 2; wx += 6)
        tft.fillRect(wx, wy, 3, 4, winCol);
  }

  asDrawHUD();
}

// ═══════════════════════════════════════════════════════════════════════════
// Alien spawning
// ═══════════════════════════════════════════════════════════════════════════

void asSpawnAlien() {
  asABld = random(0, asNBld);
  int bx = asBX[asABld], bw = asBW[asABld], bh = asBH[asABld];
  int bTop = AS_GROUND_Y - bh;

  // Difficulty: how many pixels stick out (visible against sky)
  // More score → less overhang → harder to spot
  int overhang = 5 - min(3, asScore / 5);
  if (overhang < 2) overhang = 2;

  // Decide rooftop vs side (side gets more likely at higher scores)
  int sideChance = min(50, 15 + asScore * 2);
  asATop = (bw < AS_AL_W + 4) ? true : (random(0, 100) >= sideChance);

  if (asATop) {
    // Rooftop: alien overlaps building top, 'overhang' rows above roofline
    asAY = bTop - overhang;

    // Random X along the building
    int maxX = bx + bw - AS_AL_W;
    int minX = bx;
    if (maxX < minX) maxX = minX;
    asAX = minX + random(0, max(1, maxX - minX + 1));
  } else {
    // Side: alien clings to left or right edge, 'overhang' cols sticking out
    bool left = (bx > AS_AL_W) && (random(0, 2) == 0 || bx + bw > 128 - AS_AL_W);
    if (left)
      asAX = bx - overhang;
    else
      asAX = bx + bw - AS_AL_W + overhang;

    // Random Y on the building face
    int minY = max(bTop + 2, (int)AS_HUD_H);
    int maxY = AS_GROUND_Y - AS_AL_H - 2;
    if (maxY < minY) maxY = minY;
    asAY = minY + random(0, max(1, maxY - minY + 1));
  }

  // Clamp to screen
  if (asAX < 0)              asAX = 0;
  if (asAX > 128 - AS_AL_W)  asAX = 128 - AS_AL_W;
  if (asAY < AS_HUD_H)       asAY = AS_HUD_H;
  if (asAY > 128 - AS_AL_H)  asAY = 128 - AS_AL_H;

  asDrawAlien();
}

// ═══════════════════════════════════════════════════════════════════════════
// Init / Setup / Draw / Update / Input
// ═══════════════════════════════════════════════════════════════════════════

void initAlienShoot() {
  Preferences p;
  p.begin("alienshoot", true);
  asHi = p.getInt("hiscore", 0);
  p.end();
}

void setupAlienShoot() {
  asScore = 0;
  asMissCD = false;
  asGameOver = false;
  asExpActive = false;
  asLastUpd = millis();

  asCX = 64; asCY = 64;
  asPX = asCX; asPY = asCY;

  asGenBuildings();
  asDrawScene();
  asSpawnAlien();
  asHuntStart = millis();
  asDrawCross();
}

void drawAlienShoot() {
  if (asGameOver) {
    tft.fillRect(14, 40, 100, 48, COLOR_POPUP_BG);
    tft.drawRect(14, 40, 100, 48, COLOR_BORDER);
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(25, 60);
    u8g2_display.print("AIKA LOPPUI!");
    u8g2_display.setCursor(25, 80);
    u8g2_display.print("HI: "); u8g2_display.print(asHi);
    return;
  }
  asDrawScene();
  if (!asExpActive) asDrawAlien();
  asDrawCross();
}

void updateAlienShoot() {
  if (asGameOver) return;
  if (millis() - asLastUpd < AS_FRAME_MS) return;
  asLastUpd = millis();

  // Miss cooldown timer
  if (asMissCD) {
    if (millis() - asMissStart >= AS_MISS_MS) {
      asMissCD = false;
      asEraseCross();
      asPX = asCX; asPY = asCY;
      asDrawCross();
    }
  }

  // Hunt timer — ran out of time?
  if (!asExpActive && (int)(millis() - asHuntStart) >= asHuntTime()) {
    asGameOver = true;
    if (asScore > asHi) {
      asHi = asScore;
      Preferences p;
      p.begin("alienshoot", false);
      p.putInt("hiscore", asHi);
      p.end();
    }
    drawAlienShoot();
    return;
  }

  // Update timer bar
  if (!asExpActive) asDrawHUD();

  // Explosion animation
  if (asExpActive) {
    int fr = (millis() - asExpStart) / AS_EXP_MS;
    if (fr != asExpF) {
      asExpF = fr;
      if      (fr == 1) tft.fillCircle(asExpCX, asExpCY, 3, 0xFFE0);
      else if (fr == 2) tft.fillCircle(asExpCX, asExpCY, 5, 0xFD20);
      else if (fr == 3) tft.fillCircle(asExpCX, asExpCY, 7, 0xF800);
      else if (fr >= AS_EXP_FRAMES) {
        asExpActive = false;
        asDrawScene();       // full redraw clears explosion debris
        asSpawnAlien();
        asHuntStart = millis(); // reset hunt timer for new alien
        asDrawCross();
      }
    }
  }
}

void handleAlienShootInput(char c) {
  // Game over — Enter to restart
  if (asGameOver) {
    if (c == 0x0D || c == 0x0A) setupAlienShoot();
    return;
  }

  if (asExpActive) return;  // no input during explosion

  // Movement
  bool moved = false;
  if (c == 0xB5) { asCY -= AS_CROSS_STEP; moved = true; }
  if (c == 0xB6) { asCY += AS_CROSS_STEP; moved = true; }
  if (c == 0xB4) { asCX -= AS_CROSS_STEP; moved = true; }
  if (c == 0xB7) { asCX += AS_CROSS_STEP; moved = true; }

  // Clamp
  if (asCX < AS_CROSS_SIZE)            asCX = AS_CROSS_SIZE;
  if (asCX > 127 - AS_CROSS_SIZE)      asCX = 127 - AS_CROSS_SIZE;
  if (asCY < AS_HUD_H + AS_CROSS_SIZE) asCY = AS_HUD_H + AS_CROSS_SIZE;
  if (asCY > 127 - AS_CROSS_SIZE)      asCY = 127 - AS_CROSS_SIZE;

  if (moved) {
    asEraseCross();
    asPX = asCX; asPY = asCY;
    asDrawCross();
  }

  // Shoot (Enter) — blocked during miss cooldown
  if ((c == 0x0D || c == 0x0A) && !asMissCD) {
    // Hit test: crosshair center inside alien bounding box (generous)
    bool hit = (asCX >= asAX - 1 && asCX <= asAX + AS_AL_W &&
                asCY >= asAY - 1 && asCY <= asAY + AS_AL_H);

    if (hit) {
      asScore++;
      if (asScore > asHi) {
        asHi = asScore;
        Preferences p;
        p.begin("alienshoot", false);
        p.putInt("hiscore", asHi);
        p.end();
      }
      // Start explosion at alien center
      asExpCX = asAX + AS_AL_W / 2;
      asExpCY = asAY + AS_AL_H / 2;
      asExpActive = true;
      asExpF = 0;
      asExpStart = millis();
      asDrawHUD();
    } else {
      // Miss → brief cooldown (crosshair turns red, can't shoot)
      asMissCD = true;
      asMissStart = millis();
      asEraseCross();
      asPX = asCX; asPY = asCY;
      asDrawCross();
    }
  }
}

#endif
