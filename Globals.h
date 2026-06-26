#ifndef GLOBALS_H
#define GLOBALS_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Preferences.h>

// --- PIN DEFINITIONS ---
#define TFT_SCLK 20
#define TFT_MOSI 21 
#define TFT_CS   15
#define TFT_DC   14
#define TFT_RST  18
#define KB_SDA   22
#define KB_SCL   23
#define KB_ADDR  0x5F 
#define BATT_PIN 0

// --- COLOR PALETTE ---
#define COLOR_BG        0x0000 
#define COLOR_BORDER    0x5AEB 
#define COLOR_GRAY      0x39E7 
#define COLOR_YELLOW    0xFFE0
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_TEXT      0xFFFF 
#define COLOR_POPUP_BG  0x2104 

// --- GAME SETTINGS ---
#define NUM_GAMES 17

// --- OS MODES ---
enum Mode {
  MODE_MENU,
  MODE_SANULI,
  MODE_SNAKE,
  MODE_2048,
  MODE_TTT,
  MODE_MIINAHARAVA,
  MODE_FLAPPY,
  MODE_CHESS,
  MODE_BLACKJACK,
  MODE_SOKOBAN,
  MODE_TETRIS,
  MODE_GEODASH,
  MODE_RALLIF1,
  MODE_ALIENSHOOT,
  MODE_HIRSIPUU,
  MODE_PACMAN,
  MODE_BREAKOUT,
  MODE_NELJANSUORA
};
extern Mode currentMode;

extern Adafruit_SSD1351 tft;
extern U8G2_FOR_ADAFRUIT_GFX u8g2_display;

#endif