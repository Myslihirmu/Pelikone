#ifndef MENU_H
#define MENU_H
#include "Globals.h"

const String gameNames[NUM_GAMES] = {
  "SANULI",
  "MATOPELI",
  "2048",
  "RISTINOLLA",
  "MIINAHARAVA",
  "FLAPPY BIRD",
  "SHAKKI",
  "BLACKJACK",
  "SOKOBAN",
  "TETRIS",
  "GEODASH",
  "RALLI F1",
  "TÄHTÄÄ",
  "HIRSIPUU",
  "PAC-MAN",
  "TIILISEINÄ",
  "NELJÄN SUORA"
};

void drawMenu(int selection) {
  tft.fillScreen(COLOR_BG);
  
  // Otsikko - PELIVALIKKO
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(0, 20);
  u8g2_display.print("PELIVALIKKO");
  tft.drawFastHLine(0, 25, 128, COLOR_GRAY);

  int visibleCount = 4;
  int itemHeight = 22;
  int startY = 50;
  
  static int topItem = 0;
  if (selection < topItem) topItem = selection;
  if (selection >= topItem + visibleCount) topItem = selection - visibleCount + 1;

  for (int i = 0; i < visibleCount; i++) {
    int idx = topItem + i;
    if (idx >= NUM_GAMES) break;

    int yPos = startY + (i * itemHeight);
    
    if (idx == selection) {
      tft.fillRect(0, yPos - 16, 122, 20, 0x18C3); 
      u8g2_display.setForegroundColor(COLOR_GREEN);
      u8g2_display.setCursor(2, yPos);
      u8g2_display.print(">" + gameNames[idx]); 
    } else {
      u8g2_display.setForegroundColor(COLOR_TEXT);
      u8g2_display.setCursor(2, yPos);
      u8g2_display.print(" " + gameNames[idx]);
    }
  }

  // Vierityspalkki (Scrollbar)
  if (NUM_GAMES > visibleCount) {
    int barHeight = 80;
    int scrollPos = map(selection, 0, NUM_GAMES - 1, 0, barHeight - 10);
    tft.drawFastVLine(126, 35, barHeight, COLOR_GRAY); 
    tft.fillRect(125, 35 + scrollPos, 3, 10, COLOR_GREEN);
  }
}
#endif