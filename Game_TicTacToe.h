#ifndef GAME_TTT_H
#define GAME_TTT_H
#include "Globals.h"

int tttBoard[3][3]; 
int tttX = 0, tttY = 0, tttP = 1, tttW = 0;

void drawTTTBoard() {
  tft.fillScreen(COLOR_BG);
  tft.drawFastVLine(42, 0, 128, COLOR_GRAY); tft.drawFastVLine(85, 0, 128, COLOR_GRAY);
  tft.drawFastHLine(0, 42, 128, COLOR_GRAY); tft.drawFastHLine(0, 85, 128, COLOR_GRAY);
  for(int r=0; r<3; r++) for(int c=0; c<3; c++) {
    int px = c * 43 + 12, py = r * 43 + 32;
    if(tttBoard[r][c] == 1) { u8g2_display.setForegroundColor(COLOR_GREEN); u8g2_display.setCursor(px, py); u8g2_display.print("X"); }
    else if(tttBoard[r][c] == 2) { u8g2_display.setForegroundColor(COLOR_YELLOW); u8g2_display.setCursor(px, py); u8g2_display.print("O"); }
  }
  tft.drawRect(tttX * 43 + 2, tttY * 43 + 2, 39, 39, COLOR_TEXT);
  if(tttW > 0) {
    tft.fillRect(10, 40, 108, 48, COLOR_BG); u8g2_display.setCursor(20, 65);
    u8g2_display.print(tttW == 3 ? "DRAW!" : (tttW == 1 ? "WINNER: X" : "WINNER: O"));
  }
}

void setupTTT() { for(int r=0; r<3; r++) for(int c=0; c<3; c++) tttBoard[r][c]=0; tttW=0; tttP=1; drawTTTBoard(); }

void checkTTTW() {
  for(int i=0; i<3; i++) {
    if(tttBoard[i][0]!=0 && tttBoard[i][0]==tttBoard[i][1] && tttBoard[i][1]==tttBoard[i][2]) tttW=tttBoard[i][0];
    if(tttBoard[0][i]!=0 && tttBoard[0][i]==tttBoard[1][i] && tttBoard[1][i]==tttBoard[2][i]) tttW=tttBoard[0][i];
  }
  if(tttBoard[0][0]!=0 && tttBoard[0][0]==tttBoard[1][1] && tttBoard[1][1]==tttBoard[2][2]) tttW=tttBoard[0][0];
  if(tttBoard[0][2]!=0 && tttBoard[0][2]==tttBoard[1][1] && tttBoard[1][1]==tttBoard[2][0]) tttW=tttBoard[0][2];
  if(tttW == 0) { bool full=true; for(int r=0; r<3; r++) for(int c=0; c<3; c++) if(tttBoard[r][c]==0) full=false; if(full) tttW=3; }
}

void handleTTTInput(char c) {
  if(tttW > 0) { if(c == 0x0D || c == 0x0A) setupTTT(); return; }
  if(c == 0xB4) tttX = (tttX > 0) ? tttX - 1 : 2;
  else if(c == 0xB7) tttX = (tttX < 2) ? tttX + 1 : 0;
  else if(c == 0xB5) tttY = (tttY > 0) ? tttY - 1 : 2;
  else if(c == 0xB6) tttY = (tttY < 2) ? tttY + 1 : 0;
  else if(c == 0x0D || c == 0x0A) { if(tttBoard[tttY][tttX] == 0) { tttBoard[tttY][tttX] = tttP; tttP = (tttP == 1) ? 2 : 1; checkTTTW(); } }
  drawTTTBoard();
}
#endif