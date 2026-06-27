#include "Globals.h"
#include "Menu.h"
#include "Game_Sanuli.h"
#include "Game_Snake.h"
#include "Game_2048.h"
#include "Game_TicTacToe.h"
#include "Game_Miinaharava.h"
#include "Game_Flappy.h"
#include "Game_Chess.h"
#include "Game_Blackjack.h"
#include "Game_Sokoban.h"
#include "Game_Tetris.h"
#include "Game_GeoDash.h"
#include "Game_RalliF1.h"
#include "Game_AlienShoot.h"
#include "Game_Hirsipuu.h"
#include "Game_Pacman.h"
#include "Game_Breakout.h"
#include "Game_NeljanSuora.h"
#include "Comms.h"
#include "esp_sleep.h"

Adafruit_SSD1351 tft = Adafruit_SSD1351(128, 128, &SPI, TFT_CS, TFT_DC, TFT_RST);
U8G2_FOR_ADAFRUIT_GFX u8g2_display;

Mode currentMode = MODE_MENU;
int menuPos = 0;
unsigned long lastKeyTime = 0;

void refreshDisplay() {
  if      (currentMode == MODE_MENU)        drawMenu(menuPos);
  else if (currentMode == MODE_SANULI)      drawSanuliBoard();
  else if (currentMode == MODE_SNAKE)       drawSnake();
  else if (currentMode == MODE_2048)        draw2048Board();
  else if (currentMode == MODE_TTT)         drawTTTBoard();
  else if (currentMode == MODE_MIINAHARAVA) drawMiinaharavaBoard();
  else if (currentMode == MODE_FLAPPY)      drawFlappy();
  else if (currentMode == MODE_CHESS)       drawChessBoard();
  else if (currentMode == MODE_BLACKJACK)   drawBlackjackBoard();
  else if (currentMode == MODE_SOKOBAN)     drawSokobanBoard();
  else if (currentMode == MODE_TETRIS)      drawTetrisBoard();
  else if (currentMode == MODE_GEODASH)     drawGeoDash();
  else if (currentMode == MODE_RALLIF1)    drawRalliF1();
  else if (currentMode == MODE_ALIENSHOOT) drawAlienShoot();
  else if (currentMode == MODE_HIRSIPUU)  drawHirsipuuBoard();
  else if (currentMode == MODE_PACMAN)   drawPacman();
  else if (currentMode == MODE_BREAKOUT) drawBreakout();
  else if (currentMode == MODE_NELJANSUORA) drawNeljanSuora();
}

// --- Battery monitoring ---
#define BATT_WARN_PCT   20       // warn at/below this charge level
#define BATT_CHECK_MS   30000    // how often to sample the battery
#define BATT_REWARN_MS  60000    // min gap between repeated warnings while low

unsigned long lastBattCheck = 0;
unsigned long lastBattWarn  = 0;
bool battWarned = false;

float readBatteryVoltage()    { return (analogReadMilliVolts(BATT_PIN) / 1000.0) * 2.0; }
int   batteryPercent(float v) { return constrain((int)((v - 3.0f) / 1.2f * 100.0f), 0, 100); }

void showBatteryPopup() {
  float v = readBatteryVoltage();
  int pct = batteryPercent(v);
  int x = 24, y = 40, w = 80, h = 52;
  tft.fillRoundRect(x, y, w, h, 4, COLOR_POPUP_BG);
  tft.drawRoundRect(x, y, w, h, 4, COLOR_BORDER);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(x + 10, y + 16); u8g2_display.print("AKKU:");
  u8g2_display.setCursor(x + 10, y + 32);
  u8g2_display.print(v, 2); u8g2_display.print(" V");
  u8g2_display.setCursor(x + 10, y + 47);
  u8g2_display.print(pct); u8g2_display.print(" %");

  unsigned long start = millis();
  while (millis() - start < 2000) {
    Wire.requestFrom(KB_ADDR, 1);
    if (Wire.available() && Wire.read() != 0) break;
    delay(10);
  }
  // Clear the popup's footprint first: redraw funcs (e.g. Sanuli) only repaint
  // their tiles, not the gaps between them, so popup pixels there would linger.
  tft.fillRect(x, y, w, h, COLOR_BG);
  refreshDisplay();
}

// Brief, recurring low-battery warning (Finnish). Draws over the screen,
// pauses, then restores the current view — game state is untouched.
void showLowBatteryWarning() {
  int x = 4, y = 42, w = 120, h = 44;
  tft.fillRoundRect(x, y, w, h, 4, COLOR_POPUP_BG);
  tft.drawRoundRect(x, y, w, h, 4, COLOR_RED);
  u8g2_display.setForegroundColor(COLOR_RED);
  u8g2_display.setCursor(x + 8, y + 20); u8g2_display.print("AKKU VÄHISSÄ!");
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(x + 8, y + 38); u8g2_display.print("Lataa pian!");
  delay(2500);
  tft.fillRect(x, y, w, h, COLOR_BG);
  refreshDisplay();
}

void checkBatteryWarning() {
  if (millis() - lastBattCheck < BATT_CHECK_MS) return;
  lastBattCheck = millis();
  if (batteryPercent(readBatteryVoltage()) <= BATT_WARN_PCT) {
    if (!battWarned || millis() - lastBattWarn >= BATT_REWARN_MS) {
      lastBattWarn = millis();
      battWarned = true;
      showLowBatteryWarning();
    }
  } else {
    battWarned = false;   // recovered/charging — re-arm for next time
  }
}

// --- Idle power saving (hybrid) ---
#define IDLE_SLEEP_MS   900000UL    // 15 min idle -> screen off + light sleep
#define DEEP_SLEEP_MS  1800000UL    // +30 min still idle -> full deep sleep (off)
#define IDLE_POLL_US    150000ULL   // light-sleep keypad poll interval (150 ms)

// Hybrid idle sleep. After IDLE_SLEEP_MS with no keys, blank the OLED and
// light-sleep, peeking at the keypad every IDLE_POLL_US. Any key resumes the
// game right where it was. If still untouched after DEEP_SLEEP_MS more, drop to
// full deep sleep (~uA) — recover by switching the device off and back on.
void enterIdleSleep() {
  tft.sendCommand(SSD1351_CMD_DISPLAYOFF);
  unsigned long sleepStart = millis();

  while (true) {
    esp_sleep_enable_timer_wakeup(IDLE_POLL_US);
    esp_light_sleep_start();

    Wire.requestFrom(KB_ADDR, 1);
    if (Wire.available() && Wire.read() != 0) {              // woken by a key
      lastKeyTime = millis();
      refreshDisplay();                                       // redraw while panel is off
      tft.sendCommand(SSD1351_CMD_DISPLAYON);                 // then reveal it
      return;
    }

    if (millis() - sleepStart >= DEEP_SLEEP_MS) {            // still idle -> full off
      esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL); // no timer wake
      esp_deep_sleep_start();                                 // never returns
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BATT_PIN, INPUT);
  Wire.begin(KB_SDA, KB_SCL);
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.begin();
  u8g2_display.begin(tft);
  u8g2_display.setFont(u8g2_font_helvB12_te);
  u8g2_display.setFontMode(1);
  randomSeed(esp_random());

  initSanuli();
  initSnake();
  init2048();       // FIX: was setup2048() — only load high score at boot; game setup runs on launch
  initMiinaharava();
  initFlappy();
  initBlackjack();
  initSokoban();
  initTetris();
  initGeoDash();
  initRalliF1();
  initAlienShoot();
  initHirsipuu();
  initPacman();
  initBreakout();
  initNeljanSuora();
  // setupChess() runs when launched

  lastKeyTime = millis();   // start the idle timer from power-on
  drawMenu(menuPos);
}

void loop() {
  Wire.requestFrom(KB_ADDR, 1);
  if (Wire.available()) {
    char c = Wire.read();
    // FIX: millis-based debounce instead of delay(100) — doesn't block Snake/Flappy updates
    if (c != 0 && millis() - lastKeyTime >= 80) {
      lastKeyTime = millis();
      if (c == 0x09) { showBatteryPopup(); return; }  // TAB → battery
      if (c == 0x1B) { if (currentMode == MODE_NELJANSUORA) neljanSuoraOnExit(); currentMode = MODE_MENU; u8g2_display.setFont(u8g2_font_helvB12_te); u8g2_display.setFontMode(1); drawMenu(menuPos); return; }  // ESC → menu

      if (currentMode == MODE_MENU) {
        if (c == 0xB6) {
          menuPos = (menuPos + 1) % NUM_GAMES;
          drawMenu(menuPos);
        } else if (c == 0xB5) {
          menuPos = (menuPos > 0) ? menuPos - 1 : NUM_GAMES - 1;
          drawMenu(menuPos);
        } else if (c == 0x0D || c == 0x0A) {  // ENTER
          if      (menuPos == 0) { currentMode = MODE_SANULI;      startNewSanuliGame(); }
          else if (menuPos == 1) { currentMode = MODE_SNAKE;       setupSnake(); }
          else if (menuPos == 2) { currentMode = MODE_2048;        setup2048(); }
          else if (menuPos == 3) { currentMode = MODE_TTT;         setupTTT(); }
          else if (menuPos == 4) { currentMode = MODE_MIINAHARAVA; setupMiinaharava(); }
          else if (menuPos == 5) { currentMode = MODE_FLAPPY;      setupFlappy(); }
          else if (menuPos == 6) { currentMode = MODE_CHESS;       setupChess(); }
          else if (menuPos == 7) { currentMode = MODE_BLACKJACK;   setupBlackjack(); }
          else if (menuPos == 8) { currentMode = MODE_SOKOBAN;     setupSokoban(); }
          else if (menuPos == 9) { currentMode = MODE_TETRIS;      setupTetris(); }
          else if (menuPos == 10) { currentMode = MODE_GEODASH;   gdPhase = GD_SUBMENU; drawGeoDash(); }
          else if (menuPos == 11) { currentMode = MODE_RALLIF1;  setupRalliF1(); }
          else if (menuPos == 12) { currentMode = MODE_ALIENSHOOT; setupAlienShoot(); }
          else if (menuPos == 13) { currentMode = MODE_HIRSIPUU;  setupHirsipuu(); }
          else if (menuPos == 14) { currentMode = MODE_PACMAN;    setupPacman(); }
          else if (menuPos == 15) { currentMode = MODE_BREAKOUT;  setupBreakout(); }
          else if (menuPos == 16) { currentMode = MODE_NELJANSUORA; setupNeljanSuora(); }
        }
      }
      else if (currentMode == MODE_SANULI)      handleSanuliInput(c);
      else if (currentMode == MODE_SNAKE)       handleSnakeInput(c);
      else if (currentMode == MODE_2048)        handle2048Input(c);
      else if (currentMode == MODE_TTT)         handleTTTInput(c);
      else if (currentMode == MODE_MIINAHARAVA) handleMiinaharavaInput(c);
      else if (currentMode == MODE_FLAPPY)      handleFlappyInput(c);
      else if (currentMode == MODE_CHESS)       handleChessInput(c);
      else if (currentMode == MODE_BLACKJACK)   handleBlackjackInput(c);
      else if (currentMode == MODE_SOKOBAN)     handleSokobanInput(c);
      else if (currentMode == MODE_TETRIS)      handleTetrisInput(c);
      else if (currentMode == MODE_GEODASH)     handleGeoDashInput(c);
      else if (currentMode == MODE_RALLIF1)    handleRalliF1Input(c);
      else if (currentMode == MODE_ALIENSHOOT) handleAlienShootInput(c);
      else if (currentMode == MODE_HIRSIPUU)  handleHirsipuuInput(c);
      else if (currentMode == MODE_PACMAN)   handlePacmanInput(c);
      else if (currentMode == MODE_BREAKOUT) handleBreakoutInput(c);
      else if (currentMode == MODE_NELJANSUORA) handleNeljanSuoraInput(c);
    }
  }

  if (currentMode == MODE_SNAKE)  updateSnake();
  if (currentMode == MODE_FLAPPY) updateFlappy();
  if (currentMode == MODE_TETRIS)  updateTetris();
  if (currentMode == MODE_GEODASH) updateGeoDash();
  if (currentMode == MODE_RALLIF1) updateRalliF1();
  if (currentMode == MODE_ALIENSHOOT) updateAlienShoot();
  if (currentMode == MODE_PACMAN) updatePacman();
  if (currentMode == MODE_BREAKOUT) updateBreakout();
  if (currentMode == MODE_NELJANSUORA) updateNeljanSuora();
  checkBatteryWarning();
  if (millis() - lastKeyTime >= IDLE_SLEEP_MS) enterIdleSleep();
  delay(10);
}
