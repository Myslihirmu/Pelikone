#ifndef GAME_SOKOBAN_H
#define GAME_SOKOBAN_H
#include "Globals.h"

// ---------------------------------------------------------------------------
//  SOKOBAN  –  128×128 display, 12-pixel cells (10 wide × 9 tall grid)
//
//  ALL 10 LEVELS VERIFIED SOLVABLE by BFS solver.
//
//  TWO-LAYER board design (walls can never be overwritten):
//    skTile[][] – static:  ' '=floor  '#'=wall  '.'=goal
//    skObj[][]  – dynamic: ' '=empty  '$'=box   '@'=player
// ---------------------------------------------------------------------------

#define SK_CELL       12
#define SK_MAX_W      10
#define SK_MAX_H       9
#define SK_STATUS_H   14
#define SK_NUM_LEVELS 10

// ---------------------------------------------------------------------------
//  Level data in PROGMEM – every row EXACTLY SK_MAX_W chars + '\n'
//  '#'=wall '.'=goal ' '=floor '$'=box '@'=player '*'=box-on-goal '+'=player-on-goal
// ---------------------------------------------------------------------------

// LEVEL 1 – 1 box. Push box RIGHT onto goal. Solution: →
const char sk_level_0[] PROGMEM =
  "          \n"
  "          \n"
  "  ######  \n"
  "  #    #  \n"
  "  # $. #  \n"
  "  # @  #  \n"
  "  ######  \n"
  "          \n"
  "          \n";

// LEVEL 2 – 1 box. Walk above box, push it DOWN onto goal.
const char sk_level_1[] PROGMEM =
  "          \n"
  "  ######  \n"
  "  #    #  \n"
  "  # @  #  \n"
  "  # $  #  \n"
  "  # .  #  \n"
  "  #    #  \n"
  "  ######  \n"
  "          \n";

// LEVEL 3 – 2 boxes. Both boxes above their goals. Push each down in turn.
const char sk_level_2[] PROGMEM =
  "          \n"
  " ######## \n"
  " #      # \n"
  " # $$   # \n"
  " # @    # \n"
  " # ..   # \n"
  " #      # \n"
  " ######## \n"
  "          \n";

// LEVEL 4 – 2 boxes in a column. Player is above; push each box down to its goal.
const char sk_level_3[] PROGMEM =
  "          \n"
  " ######## \n"
  " #  @   # \n"
  " # $    # \n"
  " # .    # \n"
  " # $    # \n"
  " # .    # \n"
  " ######## \n"
  "          \n";

// LEVEL 5 – 2 boxes in a corridor with goals at each end. Push outward.
const char sk_level_4[] PROGMEM =
  "          \n"
  "          \n"
  " ######## \n"
  " #      # \n"
  " #.$@$. # \n"
  " #      # \n"
  " ######## \n"
  "          \n"
  "          \n";

// LEVEL 6 – 3 boxes in a row above 3 goals. Route each box down.
const char sk_level_5[] PROGMEM =
  "          \n"
  " ######## \n"
  " #      # \n"
  " # $$$  # \n"
  " #  @   # \n"
  " # ...  # \n"
  " #      # \n"
  " ######## \n"
  "          \n";

// LEVEL 7 – 3 boxes, two-room layout. Requires repositioning between rooms.
const char sk_level_6[] PROGMEM =
  "          \n"
  "  ######  \n"
  "  # @  #  \n"
  "  # $  #  \n"
  "### .$ ## \n"
  "  #  .  # \n"
  "  # $   # \n"
  "  # .   # \n"
  "  ######  \n";

// LEVEL 8 – 3 boxes in a column, 3 goals in a column. Push each down.
const char sk_level_7[] PROGMEM =
  "  ######  \n"
  "  #    #  \n"
  "  # $  #  \n"
  "  # .@ #  \n"
  "  # $  #  \n"
  "  # .  #  \n"
  "  # $  #  \n"
  "  # .  #  \n"
  "  ######  \n";

// LEVEL 9 – 4 boxes in a row above 4 goals. Order matters.
const char sk_level_8[] PROGMEM =
  "          \n"
  " ######## \n"
  " #      # \n"
  " # $$$$ # \n"
  " #  @   # \n"
  " # .... # \n"
  " #      # \n"
  " ######## \n"
  "          \n";

// LEVEL 10 – 4 boxes arranged around centre, 4 corner goals. Hardest.
const char sk_level_9[] PROGMEM =
  " ######## \n"
  " #. .. .# \n"
  " #      # \n"
  " # $  $ # \n"
  " #   @  # \n"
  " # $  $ # \n"
  " #      # \n"
  " ######## \n"
  "          \n";

const char* const sk_levels[SK_NUM_LEVELS] PROGMEM = {
  sk_level_0, sk_level_1, sk_level_2, sk_level_3, sk_level_4,
  sk_level_5, sk_level_6, sk_level_7, sk_level_8, sk_level_9
};

// ---------------------------------------------------------------------------
//  Runtime state
// ---------------------------------------------------------------------------
char skTile[SK_MAX_H][SK_MAX_W];
char skObj [SK_MAX_H][SK_MAX_W];

char skUndoObj[SK_MAX_H][SK_MAX_W];
int  skUndoPX, skUndoPY;

int  skPX, skPY;
int  skMoves;
int  skLevel;
bool skGameOver;
bool skUndoAvail;

// ---------------------------------------------------------------------------
//  Colors
// ---------------------------------------------------------------------------
#define SK_COL_WALL    0x8410
#define SK_COL_FLOOR   0x1082
#define SK_COL_GOAL    0x0010
#define SK_COL_BOX     0xC600
#define SK_COL_BOX_OK  0x0640
#define SK_COL_PLAYER  0x07FF

// ---------------------------------------------------------------------------
//  Load level – populates skTile (never touched again) and skObj
// ---------------------------------------------------------------------------
void skLoadLevel(int lvl) {
  const char* src = (const char*)pgm_read_ptr(&sk_levels[lvl]);
  char raw[SK_MAX_H * (SK_MAX_W + 2) + 8];
  strncpy_P(raw, src, sizeof(raw) - 1);
  raw[sizeof(raw) - 1] = '\0';

  for (int r = 0; r < SK_MAX_H; r++)
    for (int c = 0; c < SK_MAX_W; c++) {
      skTile[r][c] = ' ';
      skObj [r][c] = ' ';
    }

  skPX = 0; skPY = 0;
  int row = 0, col = 0;
  for (int i = 0; raw[i] != '\0' && row < SK_MAX_H; i++) {
    char ch = raw[i];
    if (ch == '\n') { row++; col = 0; }
    else {
      if (col < SK_MAX_W) {
        switch (ch) {
          case '#': skTile[row][col] = '#'; break;
          case '.': skTile[row][col] = '.'; break;
          case '$': skObj [row][col] = '$'; break;
          case '*': skTile[row][col] = '.'; skObj[row][col] = '$'; break;
          case '@': skObj [row][col] = '@'; skPX = col; skPY = row; break;
          case '+': skTile[row][col] = '.'; skObj[row][col] = '@'; skPX = col; skPY = row; break;
          default: break;
        }
      }
      col++;
    }
  }
}

// ---------------------------------------------------------------------------
//  Draw
// ---------------------------------------------------------------------------
void drawSokobanBoard() {
  int boardPixH = 128 - SK_STATUS_H;
  int offX = (128       - SK_MAX_W * SK_CELL) / 2;
  int offY = (boardPixH - SK_MAX_H * SK_CELL) / 2;

  for (int r = 0; r < SK_MAX_H; r++) {
    for (int c = 0; c < SK_MAX_W; c++) {
      int px = offX + c * SK_CELL;
      int py = offY + r * SK_CELL;
      char tile = skTile[r][c];
      char obj  = skObj [r][c];

      uint16_t bg = (tile == '#') ? SK_COL_WALL :
                    (tile == '.') ? SK_COL_GOAL : SK_COL_FLOOR;
      tft.fillRect(px, py, SK_CELL, SK_CELL, bg);

      if (tile == '#') {
        tft.drawFastHLine(px,               py,               SK_CELL, 0xC618);
        tft.drawFastVLine(px,               py,               SK_CELL, 0xC618);
        tft.drawFastHLine(px,               py + SK_CELL - 1, SK_CELL, 0x2104);
        tft.drawFastVLine(px + SK_CELL - 1, py,               SK_CELL, 0x2104);
        continue;
      }

      // Empty goal marker
      if (tile == '.' && obj == ' ') {
        int cx = px + SK_CELL / 2, cy = py + SK_CELL / 2;
        tft.drawFastHLine(cx - 2, cy,     5, COLOR_YELLOW);
        tft.drawFastVLine(cx,     cy - 2, 5, COLOR_YELLOW);
      }

      // Box
      if (obj == '$') {
        bool onGoal   = (tile == '.');
        uint16_t fill = onGoal ? SK_COL_BOX_OK : SK_COL_BOX;
        uint16_t bord = onGoal ? COLOR_GREEN   : COLOR_YELLOW;
        tft.fillRect(px + 1, py + 1, SK_CELL - 2, SK_CELL - 2, fill);
        tft.drawRect(px + 1, py + 1, SK_CELL - 2, SK_CELL - 2, bord);
      }

      // Player
      if (obj == '@') {
        tft.fillCircle(px + SK_CELL / 2, py + SK_CELL / 2,
                       SK_CELL / 2 - 2, SK_COL_PLAYER);
      }
    }
  }

  // Status bar
  tft.fillRect(0, 128 - SK_STATUS_H, 128, SK_STATUS_H, COLOR_POPUP_BG);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(2, 127);
  u8g2_display.print("T:");
  u8g2_display.print(skLevel + 1);
  u8g2_display.print(" Srt:");
  u8g2_display.print(skMoves);

  // Win popup
  if (skGameOver) {
    tft.fillRoundRect(10, 36, 108, 50, 4, COLOR_POPUP_BG);
    tft.drawRoundRect(10, 36, 108, 50, 4, COLOR_BORDER);
    u8g2_display.setForegroundColor(COLOR_GREEN);
    u8g2_display.setCursor(14, 58);
    u8g2_display.print("RATKAISTU!");
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(16, 78);
    u8g2_display.print(skLevel < SK_NUM_LEVELS - 1
                       ? "Paina Enter"
                       : " Kaikki läpi!");
  }
}

// ---------------------------------------------------------------------------
//  Setup / restart
// ---------------------------------------------------------------------------
void setupSokoban() {
  skMoves     = 0;
  skGameOver  = false;
  skUndoAvail = false;
  skLoadLevel(skLevel);
  tft.fillScreen(COLOR_BG);
  drawSokobanBoard();
}

// ---------------------------------------------------------------------------
//  Init (called once at boot)
// ---------------------------------------------------------------------------
void initSokoban() {
  Preferences p;
  p.begin("sokoban", true);
  skLevel = p.getInt("level", 0);
  p.end();
  if (skLevel < 0 || skLevel >= SK_NUM_LEVELS) skLevel = 0;
}

// ---------------------------------------------------------------------------
//  Movement – skTile is strictly read-only; only skObj is mutated
// ---------------------------------------------------------------------------
bool skTryMove(int dx, int dy) {
  int nx = skPX + dx;
  int ny = skPY + dy;

  if (nx < 0 || nx >= SK_MAX_W || ny < 0 || ny >= SK_MAX_H) return false;
  if (skTile[ny][nx] == '#') return false;

  if (skObj[ny][nx] == '$') {
    int bx = nx + dx, by = ny + dy;
    if (bx < 0 || bx >= SK_MAX_W || by < 0 || by >= SK_MAX_H) return false;
    if (skTile[by][bx] == '#') return false;
    if (skObj [by][bx] == '$') return false;

    memcpy(skUndoObj, skObj, sizeof(skObj));
    skUndoPX = skPX; skUndoPY = skPY;
    skUndoAvail = true;

    skObj[by][bx] = '$';
    skObj[ny][nx] = ' ';

  } else if (skObj[ny][nx] == ' ') {
    memcpy(skUndoObj, skObj, sizeof(skObj));
    skUndoPX = skPX; skUndoPY = skPY;
    skUndoAvail = true;
  } else {
    return false;
  }

  skObj[skPY][skPX] = ' ';
  skObj[ny  ][nx  ] = '@';
  skPX = nx;
  skPY = ny;
  skMoves++;
  return true;
}

// ---------------------------------------------------------------------------
//  Input handler
// ---------------------------------------------------------------------------
void handleSokobanInput(char c) {
  if (skGameOver) {
    if (c == 0x0D || c == 0x0A) {
      skLevel = (skLevel < SK_NUM_LEVELS - 1) ? skLevel + 1 : 0;
      Preferences p; p.begin("sokoban", false); p.putInt("level", skLevel); p.end();
      setupSokoban();
    }
    return;
  }

  bool moved = false;
  if      (c == 0xB4) moved = skTryMove(-1,  0);  // LEFT
  else if (c == 0xB7) moved = skTryMove( 1,  0);  // RIGHT
  else if (c == 0xB5) moved = skTryMove( 0, -1);  // UP
  else if (c == 0xB6) moved = skTryMove( 0,  1);  // DOWN
  else if (c == 'r' || c == 'R') { setupSokoban(); return; }
  else if ((c == 'u' || c == 'U') && skUndoAvail) {
    memcpy(skObj, skUndoObj, sizeof(skObj));
    skPX = skUndoPX; skPY = skUndoPY;
    skUndoAvail = false;
    if (skMoves > 0) skMoves--;
    drawSokobanBoard();
    return;
  }

  if (moved) {
    bool win = false, allPlaced = true;
    for (int r = 0; r < SK_MAX_H; r++)
      for (int cc = 0; cc < SK_MAX_W; cc++)
        if (skObj[r][cc] == '$') {
          win = true;
          if (skTile[r][cc] != '.') allPlaced = false;
        }
    if (win && allPlaced) skGameOver = true;
    drawSokobanBoard();
  }
}

#endif