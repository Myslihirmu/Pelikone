#ifndef GAME_2048_H
#define GAME_2048_H
#include "Globals.h"

int board[4][4];
int score2048 = 0;
int highScore2048 = 0;
bool gameOver2048 = false;

uint16_t getTileColor(int val) {
  switch(val) {
    case 2:    return 0xC71F; case 4:    return 0xFFB6;
    case 8:    return 0xF526; case 16:   return 0xF3E0;
    case 32:   return 0xF2A0; case 64:   return 0xF100;
    case 128:  return 0xFD20; case 256:  return 0xFD20;
    case 512:  return 0xFD20; case 1024: return 0xFD20;
    case 2048: return 0xE7E0; default:   return 0x3186;
  }
}

void draw2048Board() {
  tft.fillScreen(COLOR_BG);
  for(int r=0; r<4; r++) {
    for(int c=0; c<4; c++) {
      int x = c * 32; int y = r * 32;
      uint16_t color = getTileColor(board[r][c]);
      tft.fillRect(x, y, 31, 31, color);
      if(board[r][c] > 0) {
        u8g2_display.setForegroundColor(board[r][c] <= 4 ? 0x0000 : 0xFFFF);
        String s = String(board[r][c]);
        int offset = (s.length() == 1) ? 11 : (s.length() == 2) ? 7 : (s.length() == 3) ? 3 : 1;
        u8g2_display.setCursor(x + offset, y + 21);
        u8g2_display.print(s);
      }
    }
  }
  if(gameOver2048) {
    tft.fillRect(10, 30, 108, 70, COLOR_BG); tft.drawRect(10, 30, 108, 70, COLOR_TEXT);
    u8g2_display.setForegroundColor(COLOR_GREEN); u8g2_display.setCursor(20, 55); u8g2_display.print("PELI OHI!");
    u8g2_display.setForegroundColor(COLOR_TEXT); u8g2_display.setCursor(20, 85); u8g2_display.print("Score: "); u8g2_display.print(score2048);
  }
}

void addRandomTile() {
  int empty[16][2]; int count = 0;
  for(int r=0; r<4; r++) for(int c=0; c<4; c++) if(board[r][c]==0) { empty[count][0]=r; empty[count][1]=c; count++; }
  if(count > 0) { int i = random(0, count); board[empty[i][0]][empty[i][1]] = (random(0,10) < 9) ? 2 : 4; }
}

void init2048() { Preferences p; p.begin("2048", true); highScore2048 = p.getInt("highscore", 0); p.end(); }

void setup2048() {
  score2048 = 0; gameOver2048 = false;
  for(int r=0; r<4; r++) for(int c=0; c<4; c++) board[r][c] = 0;
  addRandomTile(); addRandomTile(); draw2048Board();
}

bool canMove() {
  for(int r=0; r<4; r++) for(int c=0; c<4; c++) {
    if(board[r][c] == 0) return true;
    if(c < 3 && board[r][c] == board[r][c+1]) return true;
    if(r < 3 && board[r][c] == board[r+1][c]) return true;
  }
  return false;
}

void handle2048Input(char c) {
  if(gameOver2048) { if(c == 0x0D || c == 0x0A) setup2048(); return; }
  bool moved = false;
  auto slide = [&](int& a, int& b, int& curr) {
    if(b != 0 && a == 0) { a = b; b = 0; moved = true; }
    else if(b != 0 && a == b) { a *= 2; score2048 += a; b = 0; moved = true; }
  };

  if(c == 0xB4) { // LEFT
    for(int r=0; r<4; r++) {
      for(int i=0; i<3; i++) for(int j=0; j<3-i; j++) if(board[r][j]==0 && board[r][j+1]!=0) { board[r][j]=board[r][j+1]; board[r][j+1]=0; moved=true; }
      for(int j=0; j<3; j++) if(board[r][j]!=0 && board[r][j]==board[r][j+1]) { board[r][j]*=2; score2048+=board[r][j]; board[r][j+1]=0; moved=true; }
      for(int j=0; j<3; j++) if(board[r][j]==0 && board[r][j+1]!=0) { board[r][j]=board[r][j+1]; board[r][j+1]=0; moved=true; }
    }
  }
  if(c == 0xB7) { // RIGHT
    for(int r=0; r<4; r++) {
      for(int i=0; i<3; i++) for(int j=3; j>0; j--) if(board[r][j]==0 && board[r][j-1]!=0) { board[r][j]=board[r][j-1]; board[r][j-1]=0; moved=true; }
      for(int j=3; j>0; j--) if(board[r][j]!=0 && board[r][j]==board[r][j-1]) { board[r][j]*=2; score2048+=board[r][j]; board[r][j-1]=0; moved=true; }
      for(int j=3; j>0; j--) if(board[r][j]==0 && board[r][j-1]!=0) { board[r][j]=board[r][j-1]; board[r][j-1]=0; moved=true; }
    }
  }
  if(c == 0xB5) { // UP
    for(int col=0; col<4; col++) {
      for(int i=0; i<3; i++) for(int j=0; j<3-i; j++) if(board[j][col]==0 && board[j+1][col]!=0) { board[j][col]=board[j+1][col]; board[j+1][col]=0; moved=true; }
      for(int j=0; j<3; j++) if(board[j][col]!=0 && board[j][col]==board[j+1][col]) { board[j][col]*=2; score2048+=board[j][col]; board[j+1][col]=0; moved=true; }
      for(int j=0; j<3; j++) if(board[j][col]==0 && board[j+1][col]!=0) { board[j][col]=board[j+1][col]; board[j+1][col]=0; moved=true; }
    }
  }
  if(c == 0xB6) { // DOWN
    for(int col=0; col<4; col++) {
      for(int i=0; i<3; i++) for(int j=3; j>0; j--) if(board[j][col]==0 && board[j-1][col]!=0) { board[j][col]=board[j-1][col]; board[j-1][col]=0; moved=true; }
      for(int j=3; j>0; j--) if(board[j][col]!=0 && board[j][col]==board[j-1][col]) { board[j][col]*=2; score2048+=board[j][col]; board[j-1][col]=0; moved=true; }
      for(int j=3; j>0; j--) if(board[j][col]==0 && board[j-1][col]!=0) { board[j][col]=board[j-1][col]; board[j-1][col]=0; moved=true; }
    }
  }

  if(moved) {
    addRandomTile(); draw2048Board();
    if(!canMove()) {
      gameOver2048 = true;
      if(score2048 > highScore2048) { highScore2048 = score2048; Preferences p; p.begin("2048", false); p.putInt("highscore", highScore2048); p.end(); }
      draw2048Board();
    }
  }
}
#endif