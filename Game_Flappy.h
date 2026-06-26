#ifndef GAME_FLAPPY_H
#define GAME_FLAPPY_H
#include "Globals.h"

// Pelin vakiot
#define PIPE_WIDTH 15
#define PIPE_GAP 45
#define BIRD_X 30
#define SKY_COLOR 0x761F

// Säädettävät fysiikat (voit muuttaa näitä jos peli tuntuu yhä oudolta)
#define GRAVITY 0.25      // Pienempi luku = hitaampi putoaminen
#define JUMP_STRENGTH -2.8 // Pienempi negatiivinen luku = matalampi hyppy
#define FRAME_DELAY 30     // Päivitysväli millisekunteina (30ms = n. 33 FPS)

// Muuttujat
float birdY = 64.0;
float birdVel = 0.0;
int pipeX = 128;
int pipeGapY = 50;
int flappyScore = 0;
int flappyHiScore = 0;
bool flappyGameOver = false;

unsigned long lastFlappyUpdate = 0;
int prevBirdY = 64;
int prevPipeX = 128;

void initFlappy() {
  Preferences p;
  p.begin("flappy", true);
  flappyHiScore = p.getInt("hiscore", 0);
  p.end();
}

void setupFlappy() {
  birdY = 64.0;
  birdVel = 0.0;
  pipeX = 128;
  pipeGapY = random(20, 70);
  flappyScore = 0;
  flappyGameOver = false;
  lastFlappyUpdate = millis();
  
  prevBirdY = (int)birdY;
  prevPipeX = pipeX;

  tft.fillScreen(SKY_COLOR);
}

void drawFlappy() {
  if (flappyGameOver) {
    tft.fillRect(14, 40, 100, 48, COLOR_POPUP_BG);
    tft.drawRect(14, 40, 100, 48, COLOR_BORDER);
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(25, 60);
    u8g2_display.print("HÄVISIT!");
    u8g2_display.setCursor(25, 80);
    u8g2_display.print("HI: "); u8g2_display.print(flappyHiScore);
    return;
  }

  // Pyyhitään vanhat
  tft.fillCircle(BIRD_X, prevBirdY, 6, SKY_COLOR);
  tft.fillRect(prevPipeX + PIPE_WIDTH - 2, 0, 3, 128, SKY_COLOR);

  // Piirretään putket
  tft.fillRect(pipeX, 0, PIPE_WIDTH, pipeGapY, COLOR_GREEN); 
  tft.fillRect(pipeX, pipeGapY + PIPE_GAP, PIPE_WIDTH, 128 - (pipeGapY + PIPE_GAP), COLOR_GREEN); 
  
  // Piirretään lintu
  tft.fillCircle(BIRD_X, (int)birdY, 5, COLOR_YELLOW);
  tft.fillRect(BIRD_X + 2, (int)birdY - 2, 2, 2, 0x0000); 

  // Pisteet — FIX: clear score area before drawing to prevent ghost digits on digit-count change
  tft.fillRect(5, 2, 40, 14, SKY_COLOR);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(5, 15);
  u8g2_display.print(flappyScore);

  prevBirdY = (int)birdY;
  prevPipeX = pipeX;
}

void updateFlappy() {
  if (flappyGameOver) return;

  // AJASTIN: Päivitetään peliä vain FRAME_DELAY välein
  if (millis() - lastFlappyUpdate < FRAME_DELAY) return;
  lastFlappyUpdate = millis();

  // Fysiikka
  birdVel += GRAVITY;
  birdY += birdVel;

  // Putken liike (hidastettu 2 pikseliin per frame)
  pipeX -= 2;
  
  if (pipeX < -PIPE_WIDTH) {
    tft.fillRect(0, 0, PIPE_WIDTH + 2, 128, SKY_COLOR); // Puhdistus kun putki poistuu
    pipeX = 128;
    pipeGapY = random(20, 70);
    flappyScore++;
  }

  // Törmäykset
  if (birdY > 125 || birdY < 3) flappyGameOver = true;

  if (pipeX < BIRD_X + 5 && pipeX + PIPE_WIDTH > BIRD_X - 5) {
    if (birdY - 5 < pipeGapY || birdY + 5 > pipeGapY + PIPE_GAP) {
      flappyGameOver = true;
    }
  }

  if (flappyGameOver) {
    if (flappyScore > flappyHiScore) {
      flappyHiScore = flappyScore;
      Preferences p;
      p.begin("flappy", false);
      p.putInt("hiscore", flappyHiScore);
      p.end();
    }
  }

  drawFlappy();
}

void handleFlappyInput(char c) {
  if (flappyGameOver) {
    if (c == 0x0D || c == 0x0A) setupFlappy();
    return;
  }
  
  if (c == 0xB5) { // Ylös-nuoli
    birdVel = JUMP_STRENGTH;
  }
}

#endif