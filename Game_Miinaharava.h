#ifndef GAME_MINESWEEPER_H
#define GAME_MINESWEEPER_H
#include "Globals.h"

#define MS_ROWS 8
#define MS_COLS 8
#define MS_MINES 10
#define MS_CELL_SIZE 16

int msBoard[MS_ROWS][MS_COLS];
int msState[MS_ROWS][MS_COLS]; // 0=hidden, 1=revealed, 2=flagged
int msCursorX = 0, msCursorY = 0;
bool msFirstClick = true;
bool msGameOver = false;
bool msWin = false;

// Google Minesweeper Number Colors mapped to RGB565
uint16_t getMsNumberColor(int n) {
  switch(n) {
    case 1: return 0x1BBA; // Blue
    case 2: return 0x3C67; // Green
    case 3: return 0xD165; // Red
    case 4: return 0x78D4; // Purple
    case 5: return 0xF860; // Orange
    case 6: return 0x04B4; // Cyan
    case 7: return 0x0000; // Black
    case 8: return 0x738E; // Gray
    default: return 0xFFFF;
  }
}

void drawMiinaharavaBoard() {
  tft.fillScreen(COLOR_BG);
  for(int r = 0; r < MS_ROWS; r++) {
    for(int c = 0; c < MS_COLS; c++) {
      int x = c * MS_CELL_SIZE;
      int y = r * MS_CELL_SIZE;
      bool isLight = ((r + c) % 2 == 0);
      
      if(msState[r][c] == 0 || msState[r][c] == 2) {
        // Hidden state (Google Green Checkerboard)
        uint16_t color = isLight ? 0xAD6A : 0xA689; 
        tft.fillRect(x, y, MS_CELL_SIZE, MS_CELL_SIZE, color);
        
        if(msState[r][c] == 2) {
          // Draw Flag (Black pole, red flag)
          tft.fillRect(x + 5, y + 3, 2, 10, 0x0000); 
          tft.fillRect(x + 7, y + 3, 6, 5, 0xF800);  
        }
      } else {
        // Revealed state (Google Brown Checkerboard)
        uint16_t color = isLight ? 0xE613 : 0xD5D3;
        tft.fillRect(x, y, MS_CELL_SIZE, MS_CELL_SIZE, color);
        
        if(msBoard[r][c] == -1) {
          // Draw Bomb (Black ball)
          tft.fillCircle(x + 8, y + 8, 5, 0x0000);
        } else if(msBoard[r][c] > 0) {
          u8g2_display.setForegroundColor(getMsNumberColor(msBoard[r][c]));
          u8g2_display.setCursor(x + 4, y + 13);
          u8g2_display.print(msBoard[r][c]);
        }
      }
    }
  }
  
  // Draw Cursor
  tft.drawRect(msCursorX * MS_CELL_SIZE, msCursorY * MS_CELL_SIZE, MS_CELL_SIZE, MS_CELL_SIZE, COLOR_TEXT);
  tft.drawRect(msCursorX * MS_CELL_SIZE + 1, msCursorY * MS_CELL_SIZE + 1, MS_CELL_SIZE - 2, MS_CELL_SIZE - 2, COLOR_TEXT);

  // Game Over Popup
  if(msGameOver) {
    tft.fillRect(14, 40, 100, 48, COLOR_POPUP_BG);
    tft.drawRect(14, 40, 100, 48, COLOR_BORDER);
    u8g2_display.setCursor(24, 65);
    if(msWin) {
      u8g2_display.setForegroundColor(COLOR_GREEN);
      u8g2_display.print("VOITTO!");
    } else {
      u8g2_display.setForegroundColor(COLOR_TEXT);
      u8g2_display.print("HÄVISIT :(");
    }
  }
}

void placeMines(int firstX, int firstY) {
  int minesPlaced = 0;
  while(minesPlaced < MS_MINES) {
    int r = random(0, MS_ROWS);
    int c = random(0, MS_COLS);
    
    // Check if (r,c) is within the 3x3 area around the first click
    bool inSafeZone = false;
    for(int dr = -1; dr <= 1; dr++) {
      for(int dc = -1; dc <= 1; dc++) {
        if(r == firstY + dr && c == firstX + dc) inSafeZone = true;
      }
    }

    // Only place mine if it's not a mine already and NOT in the 3x3 safe zone
    if(msBoard[r][c] != -1 && !inSafeZone) {
      msBoard[r][c] = -1;
      minesPlaced++;
    }
  }
  
  // Calculate adjacent numbers
  for(int r = 0; r < MS_ROWS; r++) {
    for(int c = 0; c < MS_COLS; c++) {
      if(msBoard[r][c] == -1) continue;
      int count = 0;
      for(int dr = -1; dr <= 1; dr++) {
        for(int dc = -1; dc <= 1; dc++) {
          int nr = r + dr, nc = c + dc;
          if(nr >= 0 && nr < MS_ROWS && nc >= 0 && nc < MS_COLS && msBoard[nr][nc] == -1) count++;
        }
      }
      msBoard[r][c] = count;
    }
  }
}

void checkMsWin() {
  int revealed = 0;
  for(int r = 0; r < MS_ROWS; r++) {
    for(int c = 0; c < MS_COLS; c++) {
      if(msState[r][c] == 1) revealed++;
    }
  }
  if(revealed == (MS_ROWS * MS_COLS) - MS_MINES) {
    msWin = true;
    msGameOver = true;
  }
}

void revealCell(int r, int c) {
  if(r < 0 || r >= MS_ROWS || c < 0 || c >= MS_COLS || msState[r][c] != 0) return;
  msState[r][c] = 1;
  
  // Flood fill empty cells
  if(msBoard[r][c] == 0) {
    for(int dr = -1; dr <= 1; dr++) {
      for(int dc = -1; dc <= 1; dc++) {
        revealCell(r + dr, c + dc);
      }
    }
  }
}

void setupMiinaharava() {
  for(int r = 0; r < MS_ROWS; r++) {
    for(int c = 0; c < MS_COLS; c++) {
      msBoard[r][c] = 0;
      msState[r][c] = 0;
    }
  }
  msCursorX = 0; msCursorY = 0;
  msFirstClick = true;
  msGameOver = false;
  msWin = false;
  drawMiinaharavaBoard();
}

void initMiinaharava() {
  // Setup logic goes here if persistent highscores are added later
}

void handleMiinaharavaInput(char c) {
  if(msGameOver) {
    if(c == 0x0D || c == 0x0A) setupMiinaharava();
    return;
  }
  
  if(c == 0xB4) msCursorX = (msCursorX > 0) ? msCursorX - 1 : MS_COLS - 1;       // LEFT
  else if(c == 0xB7) msCursorX = (msCursorX < MS_COLS - 1) ? msCursorX + 1 : 0;  // RIGHT
  else if(c == 0xB5) msCursorY = (msCursorY > 0) ? msCursorY - 1 : MS_ROWS - 1;  // UP
  else if(c == 0xB6) msCursorY = (msCursorY < MS_ROWS - 1) ? msCursorY + 1 : 0;  // DOWN
  
  // CHANGED: Now uses 'l' or 'L' instead of 'f'
  else if(c == ' ' || c == 'l' || c == 'L') { // FLAG using Space or 'L'
    if(msState[msCursorY][msCursorX] == 0) msState[msCursorY][msCursorX] = 2;
    else if(msState[msCursorY][msCursorX] == 2) msState[msCursorY][msCursorX] = 0;
  }
  
  else if(c == 0x0D || c == 0x0A) { // REVEAL using Enter
    if(msState[msCursorY][msCursorX] == 0) { 
      if(msFirstClick) {
        placeMines(msCursorX, msCursorY);
        msFirstClick = false;
      }
      if(msBoard[msCursorY][msCursorX] == -1) {
        // Hit a mine!
        msGameOver = true;
        msWin = false;
        // Reveal all mines on death
        for(int r = 0; r < MS_ROWS; r++) {
          for(int col = 0; col < MS_COLS; col++) {
            if(msBoard[r][col] == -1) msState[r][col] = 1;
          }
        }
      } else {
        revealCell(msCursorY, msCursorX);
        checkMsWin();
      }
    }
  }
  drawMiinaharavaBoard();
}
#endif