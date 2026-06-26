#ifndef GAME_GEODASH_H
#define GAME_GEODASH_H
#include "Globals.h"

// ── Layout ───────────────────────────────────────────────────────────────────
#define GD_CUBE_X    20
#define GD_GROUND_Y  104
#define GD_CEIL_Y     18    // play area starts here (HUD lives above)
#define GD_CUBE_SZ   10
#define GD_CUBE_LAND (GD_GROUND_Y - GD_CUBE_SZ)   // 94
#define GD_FRAME_MS  30

// ── Physics ──────────────────────────────────────────────────────────────────
#define GD_GRAV   0.5f
#define GD_JUMP  -5.5f    // ~22 frames airborne, ~33px peak above ground

// ── Colors ───────────────────────────────────────────────────────────────────
#define GD_COL_GND    0x4208   // ground strip
#define GD_COL_CUBE   0xFFE0   // yellow cube
#define GD_COL_INNER  0xC600   // inner square
#define GD_COL_SPIKE  0xF800   // red spikes
#define GD_COL_BLOCK  0x39E7   // grey block

// ── Obstacle types ───────────────────────────────────────────────────────────
//  0 = single spike   (10×12)   — must jump over
//  1 = double spike   (20×12)   — must jump over
//  2 = triple spike   (30×12)   — must jump over
//  3 = platform       (20×10)   — can land on top; side = die
//  4 = wall           (10×20)   — can land on top; side = die
//  5 = platform+spike (20×22)   — block with spike on top, all-kill

#define OT_SPIKE1     0
#define OT_SPIKE2     1
#define OT_SPIKE3     2
#define OT_PLAT       3   // 20 wide, 10 tall
#define OT_WALL       4   // 10 wide, 20 tall
#define OT_PLATSPIKE  5   // 20 wide, 22 tall (10 block + 12 spike)

#define GD_SW  10   // spike unit width
#define GD_SH  12   // spike height
#define GD_BH  10   // block unit height

static inline uint8_t gdObsW(uint8_t t) {
  if (t == OT_SPIKE2) return 20;
  if (t == OT_SPIKE3) return 30;
  if (t == OT_PLAT || t == OT_PLATSPIKE) return 20;
  return 10;  // OT_SPIKE1, OT_WALL
}
static inline uint8_t gdObsH(uint8_t t) {
  if (t == OT_WALL) return GD_BH * 2;
  if (t == OT_PLATSPIKE) return GD_BH + GD_SH;
  if (t == OT_PLAT) return GD_BH;
  return GD_SH;
}

// ── Level data (PROGMEM) ─────────────────────────────────────────────────────
// y = vertical offset from ground (negative = higher, 0 = ground level)
struct GDObs { uint8_t type; int8_t y; uint16_t x; };

// ═══════════════════════════════════════════════════════════════════════════════
// LEVEL 1 — "Gauntlet"   (speed 2.0, 3200 world-px, 50 obstacles)
// Jump covers 44px horizontally.
// ═══════════════════════════════════════════════════════════════════════════════
//
// A: Wake Up Call     — 4 rapid ground spikes
// B: First Highway    — PLAT(-10) chain + ground spikes below
// C: Staircase Ascent — ascending -5→-20 w/ spikes between each step
// D: Two Paths        — WALL launcher; ground spikes OR sky PLATs
// E: Spike Gauntlet   — 5 ground obstacles, pure timing
// F: Wall Corridor    — 3 walls + spikes between, PLATSPIKE finish
// G: Descending       — drop -20→-15→-10→-5 w/ spikes below
// H: Grand Finale     — triple spike, wall→PLAT(-25), platspikes
//
const GDObs GDL1[] PROGMEM = {
  // ── A: Wake Up Call (150-340) ──
  {OT_SPIKE1,    0,  150},
  {OT_SPIKE1,    0,  200},
  {OT_SPIKE1,    0,  260},
  {OT_SPIKE2,    0,  340},
  // ── B: First Highway (430-640) ──
  {OT_PLAT,    -10,  430},
  {OT_SPIKE1,    0,  450},
  {OT_PLAT,    -10,  490},
  {OT_SPIKE1,    0,  510},
  {OT_PLAT,    -10,  550},
  {OT_SPIKE1,    0,  570},
  {OT_SPIKE2,    0,  640},
  // ── C: Staircase Ascent (740-990) ──
  {OT_PLAT,     -5,  740},
  {OT_SPIKE1,    0,  775},
  {OT_PLAT,    -10,  805},
  {OT_SPIKE1,    0,  840},
  {OT_PLAT,    -15,  870},
  {OT_SPIKE1,    0,  905},
  {OT_PLAT,    -20,  940},
  {OT_SPIKE2,    0,  990},
  // ── D: Two Paths (1060-1260) ──
  {OT_WALL,      0, 1060},
  {OT_SPIKE1,    0, 1100},   // ground path
  {OT_PLAT,    -20, 1100},   // sky path
  {OT_SPIKE1,    0, 1140},   // ground
  {OT_PLAT,    -15, 1160},   // sky
  {OT_SPIKE1,    0, 1190},
  {OT_SPIKE2,    0, 1240},
  // ── E: Spike Gauntlet (1350-1590) ──
  {OT_SPIKE1,    0, 1350},
  {OT_SPIKE2,    0, 1400},
  {OT_SPIKE1,    0, 1460},
  {OT_SPIKE1,    0, 1510},
  {OT_SPIKE3,    0, 1560},
  // ── F: Wall Corridor (1660-1920) ──
  {OT_WALL,      0, 1660},
  {OT_SPIKE1,    0, 1700},
  {OT_WALL,      0, 1740},
  {OT_SPIKE1,    0, 1780},
  {OT_WALL,      0, 1820},
  {OT_SPIKE2,    0, 1860},
  {OT_PLATSPIKE, 0, 1920},
  // ── G: Descending Platforms (2020-2250) ──
  {OT_PLAT,    -20, 2020},
  {OT_SPIKE1,    0, 2060},
  {OT_PLAT,    -15, 2080},
  {OT_SPIKE2,    0, 2120},
  {OT_PLAT,    -10, 2150},
  {OT_PLAT,     -5, 2210},
  {OT_SPIKE1,    0, 2250},
  // ── H: Grand Finale (2380-3000) ──
  {OT_SPIKE3,    0, 2380},
  {OT_WALL,      0, 2480},
  {OT_PLAT,    -25, 2530},
  {OT_SPIKE2,    0, 2570},
  {OT_PLATSPIKE, 0, 2650},
  {OT_SPIKE1,    0, 2740},
  {OT_PLAT,    -15, 2800},
  {OT_SPIKE3,    0, 2870},
  {OT_PLATSPIKE, 0, 2960},
};
const uint8_t  GDL1_N   = sizeof(GDL1) / sizeof(GDObs);
const uint16_t GDL1_LEN = 3200;
const float    GDL1_SPD = 2.0f;

// ═══════════════════════════════════════════════════════════════════════════════
// LEVEL 2 — "Overdrive"   (speed 2.5, 3800 world-px, 60 obstacles)
// Jump covers 55px horizontally.
// ═══════════════════════════════════════════════════════════════════════════════
//
// A: No Warmup        — spike2→platspike→forced plat
// B: Sky Highway      — 5× PLAT(-10) chain + ground spikes
// C: Forced Elevation — platspikes block ground, sky-only route
// D: Staircase to Sky — 5 ascending steps -5→-25
// E: Platspike Maze   — PLATSPIKE(-10) blocks mid-air; low PLATs safe
// F: Wall Gauntlet    — 3 walls inc. WALL(-10), platspike ender
// G: Dual Route       — ground spike hell vs sky plat chain
// H: Relentless Finale— wall→PLAT(-25), elevated platspikes, spike gauntlet
//
const GDObs GDL2[] PROGMEM = {
  // ── A: No Warmup (140-350) ──
  {OT_SPIKE2,    0,  140},
  {OT_PLATSPIKE, 0,  230},
  {OT_PLAT,    -15,  320},
  {OT_SPIKE1,    0,  350},
  // ── B: Sky Highway (450-780) ──
  {OT_PLAT,    -10,  450},
  {OT_SPIKE1,    0,  470},
  {OT_PLAT,    -10,  510},
  {OT_SPIKE1,    0,  530},
  {OT_PLAT,    -10,  570},
  {OT_SPIKE2,    0,  590},
  {OT_PLAT,    -10,  640},
  {OT_SPIKE1,    0,  660},
  {OT_PLAT,    -10,  700},
  {OT_SPIKE2,    0,  720},
  {OT_SPIKE1,    0,  780},
  // ── C: Forced Elevation (860-1110) ──
  {OT_WALL,      0,  860},
  {OT_PLAT,    -20,  910},
  {OT_SPIKE1,    0,  920},
  {OT_PLAT,    -15,  960},
  {OT_PLATSPIKE, 0,  975},
  {OT_PLAT,    -10, 1020},
  {OT_SPIKE2,    0, 1040},
  {OT_PLAT,     -5, 1080},
  {OT_SPIKE1,    0, 1110},
  // ── D: Staircase to Sky (1190-1450) ──
  {OT_PLAT,     -5, 1190},
  {OT_SPIKE1,    0, 1230},
  {OT_PLAT,    -10, 1250},
  {OT_SPIKE1,    0, 1285},
  {OT_PLAT,    -15, 1310},
  {OT_SPIKE1,    0, 1345},
  {OT_PLAT,    -20, 1370},
  {OT_PLAT,    -25, 1430},
  // ── E: Elevated Platspike Maze (1530-1780) ──
  {OT_SPIKE2,    0, 1530},
  {OT_PLATSPIKE,-10, 1590},
  {OT_PLAT,     -5, 1630},
  {OT_SPIKE1,    0, 1665},
  {OT_PLATSPIKE,-10, 1700},
  {OT_PLAT,     -5, 1740},
  {OT_SPIKE3,    0, 1780},
  // ── F: Wall Gauntlet (1860-2170) ──
  {OT_WALL,      0, 1860},
  {OT_SPIKE1,    0, 1910},
  {OT_WALL,      0, 1950},
  {OT_SPIKE1,    0, 2000},
  {OT_WALL,    -10, 2050},
  {OT_SPIKE2,    0, 2100},
  {OT_PLATSPIKE, 0, 2170},
  // ── G: Dual Route (2260-2590) ──
  {OT_WALL,      0, 2260},
  {OT_SPIKE1,    0, 2320},   // ground path
  {OT_PLAT,    -20, 2310},   // sky path
  {OT_SPIKE2,    0, 2370},   // ground
  {OT_PLAT,    -15, 2370},   // sky
  {OT_SPIKE3,    0, 2440},   // ground
  {OT_PLAT,    -10, 2440},   // sky
  {OT_SPIKE2,    0, 2530},
  // ── H: Relentless Finale (2700-3600) ──
  {OT_WALL,      0, 2700},
  {OT_PLAT,    -25, 2755},
  {OT_PLATSPIKE,-10, 2830},
  {OT_SPIKE3,    0, 2920},
  {OT_PLAT,    -20, 3000},
  {OT_SPIKE2,    0, 3040},
  {OT_WALL,      0, 3120},
  {OT_SPIKE1,    0, 3180},
  {OT_PLATSPIKE, 0, 3250},
  {OT_PLAT,    -15, 3340},
  {OT_SPIKE3,    0, 3420},
  {OT_WALL,      0, 3520},
};
const uint8_t  GDL2_N   = sizeof(GDL2) / sizeof(GDObs);
const uint16_t GDL2_LEN = 3800;
const float    GDL2_SPD = 2.5f;

// ═══════════════════════════════════════════════════════════════════════════════
// LEVEL 3 — "Annihilation"   (speed 3.0, 4200 world-px, 67 obstacles)
// Jump covers 66px horizontally.
// ═══════════════════════════════════════════════════════════════════════════════
//
// A: Instant Death    — spike2→wall→spike→platspike, zero breathing room
// B: Sky Gauntlet     — 6 ascending/descending platforms w/ ground spikes
// C: Platspike Corr.  — ground + elevated platspikes, high plat escapes
// D: Wall Maze        — 4 walls inc. WALL(-10), spikes between
// E: Staircase Hell   — 5 steps -5→-25 w/ spikes, tight spacing
// F: Two Routes Brutal— ground spike3 gauntlet vs sky plat chain
// G: Descending Terror— drop -25→-20→-15→-10 w/ spike2/3 below
// H: Annihilation     — walls, elevated platspikes, PLAT(-25), triple spikes
//
const GDObs GDL3[] PROGMEM = {
  // ── A: Instant Death (120-350) ──
  {OT_SPIKE2,    0,  120},
  {OT_WALL,      0,  210},
  {OT_SPIKE1,    0,  265},
  {OT_PLATSPIKE, 0,  340},
  // ── B: Sky Gauntlet (440-820) ──
  {OT_PLAT,    -10,  440},
  {OT_SPIKE1,    0,  460},
  {OT_PLAT,    -15,  510},
  {OT_SPIKE2,    0,  530},
  {OT_PLAT,    -20,  580},
  {OT_SPIKE1,    0,  600},
  {OT_PLAT,    -25,  650},
  {OT_SPIKE2,    0,  670},
  {OT_PLAT,    -20,  720},
  {OT_SPIKE1,    0,  740},
  {OT_PLAT,    -10,  790},
  {OT_SPIKE3,    0,  820},
  // ── C: Platspike Corridor (920-1200) ──
  {OT_PLATSPIKE, 0,  920},
  {OT_PLAT,    -20, 1000},
  {OT_PLATSPIKE,-10, 1070},
  {OT_PLAT,    -25, 1140},
  {OT_SPIKE2,    0, 1160},
  {OT_PLATSPIKE, 0, 1200},
  // ── D: Wall Maze (1300-1620) ──
  {OT_WALL,      0, 1300},
  {OT_SPIKE1,    0, 1360},
  {OT_WALL,      0, 1400},
  {OT_SPIKE2,    0, 1460},
  {OT_WALL,    -10, 1520},
  {OT_SPIKE1,    0, 1570},
  {OT_WALL,      0, 1610},
  // ── E: Staircase from Hell (1700-1980) ──
  {OT_PLAT,     -5, 1700},
  {OT_SPIKE1,    0, 1740},
  {OT_PLAT,    -10, 1760},
  {OT_SPIKE1,    0, 1795},
  {OT_PLAT,    -15, 1820},
  {OT_SPIKE2,    0, 1855},
  {OT_PLAT,    -20, 1890},
  {OT_SPIKE1,    0, 1925},
  {OT_PLAT,    -25, 1960},
  // ── F: Two Routes Brutal (2060-2440) ──
  {OT_WALL,      0, 2060},
  {OT_SPIKE1,    0, 2120},   // ground
  {OT_PLAT,    -20, 2120},   // sky
  {OT_SPIKE2,    0, 2180},   // ground
  {OT_PLAT,    -15, 2180},   // sky
  {OT_SPIKE1,    0, 2240},   // ground
  {OT_SPIKE3,    0, 2290},   // ground
  {OT_PLAT,    -10, 2250},   // sky
  {OT_SPIKE2,    0, 2370},
  {OT_PLATSPIKE, 0, 2440},
  // ── G: Descending Terror (2540-2830) ──
  {OT_PLAT,    -25, 2540},
  {OT_SPIKE2,    0, 2570},
  {OT_PLAT,    -20, 2610},
  {OT_SPIKE1,    0, 2640},
  {OT_PLAT,    -15, 2680},
  {OT_SPIKE2,    0, 2710},
  {OT_PLAT,    -10, 2750},
  {OT_SPIKE3,    0, 2790},
  // ── H: Annihilation Finale (2900-4000) ──
  {OT_WALL,      0, 2900},
  {OT_PLATSPIKE,-10, 2970},
  {OT_PLAT,    -25, 3050},
  {OT_SPIKE3,    0, 3080},
  {OT_PLATSPIKE, 0, 3170},
  {OT_WALL,      0, 3260},
  {OT_SPIKE2,    0, 3320},
  {OT_PLAT,    -20, 3400},
  {OT_SPIKE3,    0, 3440},
  {OT_PLATSPIKE, 0, 3540},
  {OT_SPIKE3,    0, 3640},
  {OT_WALL,    -10, 3740},
  {OT_PLAT,    -25, 3810},
  {OT_SPIKE3,    0, 3880},
  {OT_PLATSPIKE, 0, 3960},
};
const uint8_t  GDL3_N   = sizeof(GDL3) / sizeof(GDObs);
const uint16_t GDL3_LEN = 4200;
const float    GDL3_SPD = 3.0f;

// ── State ─────────────────────────────────────────────────────────────────────
enum GDPhase { GD_SUBMENU, GD_PLAYING, GD_DEAD, GD_WIN };

GDPhase gdPhase     = GD_SUBMENU;
int     gdSel       = 0;
int     gdMode      = -1;   // -1=infinite, 0/1/2=level

float   gdCubeY     = GD_CUBE_LAND;
float   gdVel       = 0.0f;
bool    gdOnGround  = true;
float   gdScrollX   = 0.0f;
float   gdSpeed     = 2.0f;

int     gdScore     = 0;
int     gdHiScore   = 0;
int     gdBestPct[3] = {0, 0, 0};

int     gdAirFrames = 0;
int     gdCubeRot   = 0;

unsigned long gdLastFrame = 0;

#define GD_NSLOTS 5
struct GDSlot { uint8_t type; int8_t y; float wx; };
GDSlot gdSlots[GD_NSLOTS];
float  gdNextSpawn = 0.0f;

// Dirty-rect list: footprints of the sprites (obstacles + cube) drawn last
// frame. Each frame we erase exactly these and repaint, so the static ground
// and untouched background are never cleared → no full-screen flicker.
struct GDRect { int16_t x, y, w, h; };
#define GD_MAX_DIRTY 32
GDRect gdDirty[GD_MAX_DIRTY];
int    gdNDirty = 0;

// ── NVS ──────────────────────────────────────────────────────────────────────
void initGeoDash() {
  Preferences p; p.begin("geodash", true);
  gdHiScore    = p.getInt("hi", 0);
  gdBestPct[0] = p.getInt("l1", 0);
  gdBestPct[1] = p.getInt("l2", 0);
  gdBestPct[2] = p.getInt("l3", 0);
  p.end();
}

static void gdSaveLevelPct(int lv, int pct) {
  if (pct <= gdBestPct[lv]) return;
  gdBestPct[lv] = pct;
  Preferences p; p.begin("geodash", false);
  if      (lv == 0) p.putInt("l1", pct);
  else if (lv == 1) p.putInt("l2", pct);
  else              p.putInt("l3", pct);
  p.end();
}

// ── Collision: returns 0=none, 1=land on top, 2=die ──────────────────────────
// baseY = bottom edge of obstacle (GD_GROUND_Y + yOffset)
static int gdHitObs(int sx, uint8_t t, int baseY) {
  int cL = GD_CUBE_X + 1,           cR = GD_CUBE_X + GD_CUBE_SZ - 1;
  int cT = (int)gdCubeY + 1,        cB = (int)gdCubeY + GD_CUBE_SZ - 1;
  int oW = gdObsW(t);

  // No horizontal overlap
  if (cR <= sx || cL >= sx + oW) return 0;

  if (t <= OT_SPIKE3) {
    // Spike: per-unit inset hitbox
    int n = t + 1;
    for (int i = 0; i < n; i++) {
      int sL = sx + i * GD_SW + 2, sR = sx + i * GD_SW + GD_SW - 2;
      int sT = baseY - GD_SH, sB = baseY;
      if (cL < sR && cR > sL && cT < sB && cB > sT) return 2;
    }
    return 0;
  }

  // Platspike: block base + spike on top — all-kill, no landing
  if (t == OT_PLATSPIKE) {
    int topY = baseY - GD_BH - GD_SH;
    if (cL < sx + 18 && cR > sx + 2 && cT < baseY && cB > topY) return 2;
    return 0;
  }

  // Block (OT_PLAT or OT_WALL) — can land on top
  int bH   = (t == OT_WALL) ? GD_BH * 2 : GD_BH;
  int bTop = baseY - bH;

  // Landing from above: cube bottom near block top, cube top above block top
  if (cT <= bTop && cB >= bTop - 1) return 1;

  // Any other overlap → side / bottom hit → die
  if (cB > bTop && cT < baseY) return 2;

  return 0;
}

// ── Drawing helpers ───────────────────────────────────────────────────────────
static void gdDrawSpike(int bx, int baseY, uint16_t col) {
  tft.fillTriangle(bx, baseY,
                   bx + GD_SW, baseY,
                   bx + GD_SW / 2, baseY - GD_SH, col);
}

static void gdDrawObstacle(int sx, uint8_t t, int baseY) {
  if (sx + (int)gdObsW(t) < 0 || sx > 128) return;
  int oW = gdObsW(t);
  switch (t) {
    case OT_SPIKE1:
      gdDrawSpike(sx, baseY, GD_COL_SPIKE); break;
    case OT_SPIKE2:
      gdDrawSpike(sx, baseY, GD_COL_SPIKE);
      gdDrawSpike(sx + GD_SW, baseY, GD_COL_SPIKE); break;
    case OT_SPIKE3:
      gdDrawSpike(sx, baseY, GD_COL_SPIKE);
      gdDrawSpike(sx + GD_SW, baseY, GD_COL_SPIKE);
      gdDrawSpike(sx + GD_SW * 2, baseY, GD_COL_SPIKE); break;
    case OT_PLAT:
      tft.fillRect(sx, baseY - GD_BH, oW, GD_BH, GD_COL_BLOCK);
      tft.drawRect(sx, baseY - GD_BH, oW, GD_BH, COLOR_BORDER); break;
    case OT_WALL:
      tft.fillRect(sx, baseY - GD_BH * 2, oW, GD_BH * 2, GD_COL_BLOCK);
      tft.drawRect(sx, baseY - GD_BH * 2, oW, GD_BH * 2, COLOR_BORDER); break;
    case OT_PLATSPIKE: {
      // Block base
      tft.fillRect(sx, baseY - GD_BH, 20, GD_BH, GD_COL_BLOCK);
      tft.drawRect(sx, baseY - GD_BH, 20, GD_BH, COLOR_BORDER);
      // Spike on top of block
      int sBase = baseY - GD_BH;
      tft.fillTriangle(sx + 5, sBase, sx + 15, sBase,
                       sx + 10, sBase - GD_SH, GD_COL_SPIKE);
    } break;
  }
}

static void gdDrawCube() {
  int cy = (int)gdCubeY;
  tft.fillRect(GD_CUBE_X, cy, GD_CUBE_SZ, GD_CUBE_SZ, GD_COL_CUBE);
  // Inner square shifts corners to fake rotation while airborne
  static const int8_t ox[4] = {2, 5, 5, 2};
  static const int8_t oy[4] = {2, 2, 5, 5};
  tft.fillRect(GD_CUBE_X + ox[gdCubeRot], cy + oy[gdCubeRot], 4, 4, GD_COL_INNER);
}

// ── Dirty-rect sprite tracking ────────────────────────────────────────────────
static void gdPushDirty(int x, int y, int w, int h) {
  if (gdNDirty >= GD_MAX_DIRTY) return;
  gdDirty[gdNDirty] = { (int16_t)x, (int16_t)y, (int16_t)w, (int16_t)h };
  gdNDirty++;
}

// Erase every sprite drawn last frame back to the play-area background, leaving
// the static ground strip below and (after a later redraw) its border intact.
static void gdEraseDirty() {
  for (int i = 0; i < gdNDirty; i++) {
    int x = gdDirty[i].x, y = gdDirty[i].y, w = gdDirty[i].w, h = gdDirty[i].h;
    if (x < 0)         { w += x; x = 0; }
    if (x + w > 128)   w = 128 - x;
    if (y < GD_CEIL_Y) { h -= (GD_CEIL_Y - y); y = GD_CEIL_Y; }
    int y2 = y + h - 1;
    if (y2 > GD_GROUND_Y) y2 = GD_GROUND_Y;   // never touch the ground strip below
    if (w > 0 && y2 >= y) tft.fillRect(x, y, w, y2 - y + 1, COLOR_BG);
  }
  gdNDirty = 0;
}

// Draw an obstacle and record its footprint for next frame's erase.
static void gdDrawObstacleR(int sx, uint8_t t, int baseY) {
  if (sx + (int)gdObsW(t) < 0 || sx > 128) return;
  gdDrawObstacle(sx, t, baseY);
  gdPushDirty(sx, baseY - gdObsH(t), gdObsW(t), gdObsH(t) + 1);
}

// Draw the cube and record its footprint.
static void gdDrawCubeR() {
  gdDrawCube();
  gdPushDirty(GD_CUBE_X, (int)gdCubeY, GD_CUBE_SZ, GD_CUBE_SZ);
}

// Iterate the active obstacle set (infinite slots or the level array), drawing
// and recording each on-screen obstacle. Shared by the full and incremental draws.
static void gdDrawObstacles() {
  if (gdMode == -1) {
    for (int i = 0; i < GD_NSLOTS; i++) {
      if (gdSlots[i].wx < gdScrollX - 30.0f) continue;
      int sx = GD_CUBE_X + (int)(gdSlots[i].wx - gdScrollX);
      int baseY = GD_GROUND_Y + gdSlots[i].y;
      gdDrawObstacleR(sx, gdSlots[i].type, baseY);
    }
  } else {
    const GDObs* arr = (gdMode == 0) ? GDL1 : (gdMode == 1) ? GDL2 : GDL3;
    uint8_t cnt      = (gdMode == 0) ? GDL1_N : (gdMode == 1) ? GDL2_N : GDL3_N;
    for (int i = 0; i < cnt; i++) {
      GDObs o; memcpy_P(&o, &arr[i], sizeof(GDObs));
      int sx = GD_CUBE_X + (int)((float)o.x - gdScrollX);
      int baseY = GD_GROUND_Y + o.y;
      gdDrawObstacleR(sx, o.type, baseY);
    }
  }
}

static void gdDrawHUD() {
  tft.fillRect(0, 0, 128, GD_CEIL_Y, COLOR_BG);
  if (gdMode == -1) {
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(2, 16);  u8g2_display.print(gdScore);
    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(60, 16); u8g2_display.print("HI:");
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(83, 16); u8g2_display.print(gdHiScore);
  } else {
    uint16_t llen = (gdMode == 0) ? GDL1_LEN : (gdMode == 1) ? GDL2_LEN : GDL3_LEN;
    int pct = constrain((int)(gdScrollX * 100 / llen), 0, 100);
    tft.drawRect(2, 4, 86, 7, COLOR_BORDER);
    tft.fillRect(3, 5, (int)(84 * pct / 100), 5, COLOR_GREEN);
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(91, 16); u8g2_display.print(pct); u8g2_display.print("%");
  }
}

// Incremental per-frame render used during play: erase last frame's sprites,
// restore the ground border line (it sits behind obstacles), then repaint the
// obstacles and cube. No full-screen clear, so the scroll stays smooth.
static void gdRenderFrame() {
  gdEraseDirty();
  tft.drawFastHLine(0, GD_GROUND_Y, 128, COLOR_BORDER);
  gdDrawObstacles();
  gdDrawCubeR();
  gdDrawHUD();
}

static void gdDrawMenu() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(0, 20); u8g2_display.print("GEODASH");
  tft.drawFastHLine(0, 25, 128, COLOR_GRAY);

  const char* lbl[4] = {"INFINITE", "LEVEL 1", "LEVEL 2", "LEVEL 3"};
  for (int i = 0; i < 4; i++) {
    int y = 50 + i * 22;
    if (i == gdSel) {
      tft.fillRect(0, y - 16, 122, 20, 0x18C3);
      u8g2_display.setForegroundColor(COLOR_GREEN);
    } else {
      u8g2_display.setForegroundColor(COLOR_TEXT);
    }
    u8g2_display.setCursor(2, y);
    u8g2_display.print(i == gdSel ? ">" : " ");
    u8g2_display.print(lbl[i]);

    int stat = (i == 0) ? gdHiScore : gdBestPct[i - 1];
    if (stat > 0) {
      u8g2_display.setForegroundColor(i == gdSel ? COLOR_TEXT : COLOR_GRAY);
      u8g2_display.setCursor(86, y);
      u8g2_display.print(stat);
      if (i > 0) u8g2_display.print("%");
    }
  }
}

void drawGeoDash() {
  if (gdPhase == GD_SUBMENU) { gdDrawMenu(); return; }

  // Full-scene redraw — used at start and by refreshDisplay() (battery popup);
  // it also seeds the dirty list that the incremental gdRenderFrame() consumes.
  tft.fillRect(0, GD_CEIL_Y, 128, GD_GROUND_Y - GD_CEIL_Y, COLOR_BG);
  // Ground strip
  tft.fillRect(0, GD_GROUND_Y, 128, 128 - GD_GROUND_Y, GD_COL_GND);
  tft.drawFastHLine(0, GD_GROUND_Y, 128, COLOR_BORDER);

  gdNDirty = 0;          // whole play area just cleared → start the dirty list fresh
  gdDrawObstacles();
  gdDrawCubeR();
  gdDrawHUD();

  // Overlays
  if (gdPhase == GD_DEAD) {
    tft.fillRect(14, 38, 100, 52, COLOR_POPUP_BG);
    tft.drawRect(14, 38, 100, 52, COLOR_BORDER);
    u8g2_display.setForegroundColor(0xF800);
    u8g2_display.setCursor(28, 55); u8g2_display.print("CRASHED!");
    if (gdMode == -1) {
      u8g2_display.setForegroundColor(COLOR_TEXT);
      u8g2_display.setCursor(20, 70); u8g2_display.print("Score: "); u8g2_display.print(gdScore);
      u8g2_display.setForegroundColor(COLOR_GRAY);
      u8g2_display.setCursor(20, 82); u8g2_display.print("Best:  "); u8g2_display.print(gdHiScore);
    } else {
      uint16_t llen = (gdMode == 0) ? GDL1_LEN : (gdMode == 1) ? GDL2_LEN : GDL3_LEN;
      int pct = constrain((int)(gdScrollX * 100 / llen), 0, 100);
      u8g2_display.setForegroundColor(COLOR_TEXT);
      u8g2_display.setCursor(20, 70); u8g2_display.print(pct); u8g2_display.print("% done");
      u8g2_display.setForegroundColor(COLOR_GRAY);
      u8g2_display.setCursor(20, 82); u8g2_display.print("Best:  "); u8g2_display.print(gdBestPct[gdMode]); u8g2_display.print("%");
    }
  }

  if (gdPhase == GD_WIN) {
    tft.fillRect(14, 38, 100, 52, COLOR_POPUP_BG);
    tft.drawRect(14, 38, 100, 52, COLOR_BORDER);
    u8g2_display.setForegroundColor(COLOR_GREEN);
    u8g2_display.setCursor(18, 55); u8g2_display.print("COMPLETE!");
    u8g2_display.setForegroundColor(COLOR_YELLOW);
    u8g2_display.setCursor(18, 71); u8g2_display.print("LEVEL "); u8g2_display.print(gdMode + 1);
    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(18, 83); u8g2_display.print("Enter=back");
  }
}

// ── Setup ─────────────────────────────────────────────────────────────────────
static void gdSpawnSlot(int i, float wx) {
  static const uint8_t pool[8] = {
    OT_SPIKE1, OT_SPIKE2, OT_SPIKE2, OT_SPIKE3,
    OT_PLAT, OT_WALL, OT_PLATSPIKE, OT_SPIKE1
  };
  uint8_t t = pool[random(0, 8)];
  gdSlots[i].type = t;
  gdSlots[i].wx   = wx;
  // Assign y offset: spikes and platspikes stay on ground, blocks float
  if (t == OT_PLAT) {
    static const int8_t platY[4] = {0, -10, -15, -20};
    gdSlots[i].y = platY[random(0, 4)];
  } else if (t == OT_WALL) {
    static const int8_t wallY[3] = {0, 0, -10};
    gdSlots[i].y = wallY[random(0, 3)];
  } else {
    gdSlots[i].y = 0;
  }
}

void setupGeoDash() {
  gdCubeY     = GD_CUBE_LAND;
  gdVel       = 0.0f;
  gdOnGround  = true;
  gdScrollX   = 0.0f;
  gdScore     = 0;
  gdAirFrames = 0;
  gdCubeRot   = 0;
  gdPhase     = GD_PLAYING;
  gdLastFrame = millis();

  if (gdMode == -1) {
    gdSpeed     = 2.0f;
    gdNextSpawn = 160.0f;
    for (int i = 0; i < 3; i++) {
      gdSpawnSlot(i, gdNextSpawn);
      gdNextSpawn += (float)random(80, 151);
    }
    for (int i = 3; i < GD_NSLOTS; i++) gdSlots[i].wx = -999.0f;
  } else {
    gdSpeed = (gdMode == 0) ? GDL1_SPD : (gdMode == 1) ? GDL2_SPD : GDL3_SPD;
  }

  tft.fillScreen(COLOR_BG);
  drawGeoDash();   // paint the initial scene + seed the dirty list for gdRenderFrame()
}

// ── Update ────────────────────────────────────────────────────────────────────
void updateGeoDash() {
  if (gdPhase != GD_PLAYING) return;
  if (millis() - gdLastFrame < GD_FRAME_MS) return;
  gdLastFrame = millis();

  // 1. Advance scroll
  gdScrollX += gdSpeed;

  // 2. Physics
  gdVel   += GD_GRAV;
  gdCubeY += gdVel;

  bool onSomething = false;
  bool died        = false;

  // 3. Ground collision
  if (gdCubeY >= GD_CUBE_LAND) {
    gdCubeY     = GD_CUBE_LAND;
    gdVel       = 0.0f;
    onSomething = true;
  }

  // 4. Ceiling kill
  if (gdCubeY < (float)GD_CEIL_Y) died = true;

  // 5. Obstacle collision
  if (!died) {
    if (gdMode == -1) {
      for (int i = 0; i < GD_NSLOTS; i++) {
        if (gdSlots[i].wx < gdScrollX - 30.0f) {
          gdSpawnSlot(i, gdNextSpawn);
          gdNextSpawn += (float)random(80, 151);
          continue;
        }
        int sx    = GD_CUBE_X + (int)(gdSlots[i].wx - gdScrollX);
        int baseY = GD_GROUND_Y + gdSlots[i].y;
        if (sx < -20 || sx > 60) continue;
        int hit = gdHitObs(sx, gdSlots[i].type, baseY);
        if (hit == 2) { died = true; break; }
        if (hit == 1 && !onSomething) {
          int bH  = (gdSlots[i].type == OT_WALL) ? GD_BH * 2 : GD_BH;
          gdCubeY = (float)(baseY - bH - GD_CUBE_SZ);
          gdVel   = 0.0f;
          onSomething = true;
        }
      }
    } else {
      const GDObs* arr  = (gdMode == 0) ? GDL1 : (gdMode == 1) ? GDL2 : GDL3;
      uint8_t      cnt  = (gdMode == 0) ? GDL1_N : (gdMode == 1) ? GDL2_N : GDL3_N;
      uint16_t     llen = (gdMode == 0) ? GDL1_LEN : (gdMode == 1) ? GDL2_LEN : GDL3_LEN;

      if (gdScrollX >= llen) {
        gdPhase = GD_WIN;
        gdSaveLevelPct(gdMode, 100);
        drawGeoDash();
        return;
      }

      for (int i = 0; i < cnt; i++) {
        GDObs o; memcpy_P(&o, &arr[i], sizeof(GDObs));
        int sx    = GD_CUBE_X + (int)((float)o.x - gdScrollX);
        int baseY = GD_GROUND_Y + o.y;
        if (sx < -20 || sx > 60) continue;
        int hit = gdHitObs(sx, o.type, baseY);
        if (hit == 2) { died = true; break; }
        if (hit == 1 && !onSomething) {
          int bH  = (o.type == OT_WALL) ? GD_BH * 2 : GD_BH;
          gdCubeY = (float)(baseY - bH - GD_CUBE_SZ);
          gdVel   = 0.0f;
          onSomething = true;
        }
      }
    }
  }

  // 6. Update ground state + rotation
  gdOnGround = onSomething;
  if (!gdOnGround) {
    gdAirFrames++;
    gdCubeRot = (gdAirFrames / 3) % 4;
  } else {
    gdAirFrames = 0;
    gdCubeRot   = 0;
  }

  // 7. Infinite mode: score + speed ramp
  if (gdMode == -1) {
    gdScore = (int)gdScrollX / 10;
    gdSpeed = min(4.5f, 2.0f + (float)(gdScore / 20) * 0.3f);
  }

  // 8. Handle death
  if (died) {
    gdPhase = GD_DEAD;
    if (gdMode == -1) {
      if (gdScore > gdHiScore) {
        gdHiScore = gdScore;
        Preferences p; p.begin("geodash", false); p.putInt("hi", gdHiScore); p.end();
      }
    } else {
      uint16_t llen = (gdMode == 0) ? GDL1_LEN : (gdMode == 1) ? GDL2_LEN : GDL3_LEN;
      gdSaveLevelPct(gdMode, constrain((int)(gdScrollX * 100 / llen), 0, 100));
    }
  }

  // Still alive → smooth incremental frame; on death fall back to the full
  // redraw so the crash popup is drawn over a consistent scene.
  if (gdPhase == GD_PLAYING) gdRenderFrame();
  else drawGeoDash();
}

// ── Input ─────────────────────────────────────────────────────────────────────
void handleGeoDashInput(char c) {
  if (gdPhase == GD_SUBMENU) {
    if      (c == 0xB5) { gdSel = max(0, gdSel - 1); gdDrawMenu(); }
    else if (c == 0xB6) { gdSel = min(3, gdSel + 1); gdDrawMenu(); }
    else if (c == 0x0D || c == 0x0A) { gdMode = gdSel - 1; setupGeoDash(); }
    return;
  }
  if (gdPhase == GD_DEAD || gdPhase == GD_WIN) {
    if (c == 0x0D || c == 0x0A) { gdPhase = GD_SUBMENU; gdDrawMenu(); }
    return;
  }
  // Playing: jump on UP or SPACE
  if ((c == 0xB5 || c == ' ') && gdOnGround) {
    gdVel      = GD_JUMP;
    gdOnGround = false;
  }
}

#endif
