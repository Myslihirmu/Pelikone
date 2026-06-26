#ifndef GAME_CHESS_H
#define GAME_CHESS_H
#include "Globals.h"

// 0 = Empty
// White: 1=P, 2=N, 3=B, 4=R, 5=Q, 6=K
// Black: 11=p, 12=n, 13=b, 14=r, 15=q, 16=k
int chessBoard[8][8];
int chessTurn = 0; // 0 = White, 1 = Black
int chessCursorX = 0, chessCursorY = 0;
int chessSelX = -1, chessSelY = -1;
bool chessGameOver = false;

// Board Colors
#define CHESS_LIGHT 0xF7DE
#define CHESS_DARK  0x34A6
#define CHESS_SEL   0xF800 

void initChessBoard() {
  int initial[8][8] = {
    {14, 12, 13, 15, 16, 13, 12, 14},
    {11, 11, 11, 11, 11, 11, 11, 11},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {4, 2, 3, 5, 6, 3, 2, 4}
  };
  for(int r=0; r<8; r++) {
    for(int c=0; c<8; c++) {
      chessBoard[r][c] = initial[r][c];
    }
  }
}

// Forward declaration so setupChess can call it
void drawChessBoard();

void setupChess() {
  initChessBoard();
  chessTurn = 0;
  chessCursorX = 0; chessCursorY = 0;
  chessSelX = -1; chessSelY = -1;
  chessGameOver = false;
  
  tft.fillScreen(COLOR_BG);
  drawChessBoard(); // This line makes the game appear instantly
}

void drawChessBoard() {
  for(int r=0; r<8; r++) {
    for(int c=0; c<8; c++) {
      int x = c * 16;
      int y = r * 16;
      bool isLight = ((r + c) % 2 == 0);
      uint16_t bgColor = isLight ? CHESS_LIGHT : CHESS_DARK;
      
      if(chessSelX == c && chessSelY == r) bgColor = CHESS_SEL;
      tft.fillRect(x, y, 16, 16, bgColor);
      
      int p = chessBoard[r][c];
      if(p != 0) {
        String pieceStr = "";
        int type = p % 10;
        if(type == 1) pieceStr = "P"; else if(type == 2) pieceStr = "N";
        else if(type == 3) pieceStr = "B"; else if(type == 4) pieceStr = "R";
        else if(type == 5) pieceStr = "Q"; else if(type == 6) pieceStr = "K";
        
        // Offset to center letters (x + 2)
        int textX = x + 2; 
        int textY = y + 13;

        if (p < 10) { // WHITE PIECES (Light Gray)
          uint16_t lightGray = 0xD69A; 
          u8g2_display.setForegroundColor(0x0000); 
          u8g2_display.setCursor(textX + 1, textY + 1);
          u8g2_display.print(pieceStr);
          u8g2_display.setForegroundColor(lightGray);
          u8g2_display.setCursor(textX, textY);
          u8g2_display.print(pieceStr);
        } else { // BLACK PIECES
          u8g2_display.setForegroundColor(0xFFFF); 
          u8g2_display.setCursor(textX + 1, textY + 1);
          u8g2_display.print(pieceStr);
          u8g2_display.setForegroundColor(0x0000);
          u8g2_display.setCursor(textX, textY);
          u8g2_display.print(pieceStr);
        }
      }
    }
  }
  
  // Draw cursor
  tft.drawRect(chessCursorX * 16, chessCursorY * 16, 16, 16, COLOR_YELLOW);
  tft.drawRect(chessCursorX * 16 + 1, chessCursorY * 16 + 1, 14, 14, COLOR_YELLOW);

  if(chessGameOver) {
    tft.fillRect(14, 40, 100, 48, COLOR_POPUP_BG);
    tft.drawRect(14, 40, 100, 48, COLOR_BORDER);
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(24, 60);
    u8g2_display.print(chessTurn == 1 ? "VALKOINEN" : "MUSTA");
    u8g2_display.setCursor(35, 80);
    u8g2_display.print("VOITTI!");
  }
}

// Logic: Check if squares between start and end are empty
bool isPathClear(int sr, int sc, int dr, int dc) {
  int rDir = (dr > sr) ? 1 : ((dr < sr) ? -1 : 0);
  int cDir = (dc > sc) ? 1 : ((dc < sc) ? -1 : 0);
  int r = sr + rDir;
  int c = sc + cDir;
  while(r != dr || c != dc) {
    if(chessBoard[r][c] != 0) return false;
    r += rDir; c += cDir;
  }
  return true;
}

// Logic: Does the piece move like its type allows?
bool isPseudoLegal(int sr, int sc, int dr, int dc) {
  int p = chessBoard[sr][sc];
  if(p == 0) return false;
  int type = p % 10;
  int color = (p > 10) ? 1 : 0;
  int target = chessBoard[dr][dc];
  
  if(target != 0) {
    int tColor = (target > 10) ? 1 : 0;
    if(color == tColor) return false; 
  }
  
  int drAbs = abs(dr - sr);
  int dcAbs = abs(dc - sc);
  
  if(type == 1) { // Pawn
    int dir = (color == 0) ? -1 : 1;
    if(dc == sc && target == 0) {
      if(dr == sr + dir) return true;
      if((color == 0 && sr == 6) || (color == 1 && sr == 1)) {
        if(dr == sr + 2 * dir && chessBoard[sr + dir][sc] == 0) return true;
      }
    } else if(dcAbs == 1 && dr == sr + dir && target != 0) {
      return true;
    }
    return false;
  }
  if(type == 2) return (drAbs == 2 && dcAbs == 1) || (drAbs == 1 && dcAbs == 2);
  if(type == 3) return (drAbs == dcAbs) && isPathClear(sr, sc, dr, dc);
  if(type == 4) return (drAbs == 0 || dcAbs == 0) && isPathClear(sr, sc, dr, dc);
  if(type == 5) return ((drAbs == dcAbs) || (drAbs == 0 || dcAbs == 0)) && isPathClear(sr, sc, dr, dc);
  if(type == 6) return (drAbs <= 1 && dcAbs <= 1);
  
  return false;
}

// Logic: Is a square attacked by any piece of the attacker's color?
bool isUnderAttack(int r, int c, int defenderColor) {
  int attackerColor = 1 - defenderColor;
  for(int i=0; i<8; i++) {
    for(int j=0; j<8; j++) {
      int p = chessBoard[i][j];
      if(p != 0 && ((p > 10 ? 1 : 0) == attackerColor)) {
        if(isPseudoLegal(i, j, r, c)) return true;
      }
    }
  }
  return false;
}

// Logic: Is the king in check?
bool inCheck(int color) {
  int kr = -1, kc = -1;
  int kingVal = color == 1 ? 16 : 6;
  for(int i=0; i<8; i++) {
    for(int j=0; j<8; j++) {
      if(chessBoard[i][j] == kingVal) { kr = i; kc = j; break; }
    }
  }
  if(kr == -1) return true; 
  return isUnderAttack(kr, kc, color);
}

// Logic: Simulate move to see if king remains safe
bool tryMove(int sr, int sc, int dr, int dc) {
  if(!isPseudoLegal(sr, sc, dr, dc)) return false;
  int color = (chessBoard[sr][sc] > 10) ? 1 : 0;
  int savedTarget = chessBoard[dr][dc];
  chessBoard[dr][dc] = chessBoard[sr][sc];
  chessBoard[sr][sc] = 0;
  bool safe = !inCheck(color);
  chessBoard[sr][sc] = chessBoard[dr][dc];
  chessBoard[dr][dc] = savedTarget;
  return safe;
}

// Logic: Can this player make any legal move?
bool hasLegalMoves(int color) {
  for(int sr=0; sr<8; sr++) {
    for(int sc=0; sc<8; sc++) {
      int p = chessBoard[sr][sc];
      if(p != 0 && ((p > 10 ? 1 : 0) == color)) {
        for(int dr=0; dr<8; dr++) {
          for(int dc=0; dc<8; dc++) {
            if(tryMove(sr, sc, dr, dc)) return true;
          }
        }
      }
    }
  }
  return false;
}

void handleChessInput(char c) {
  if(chessGameOver) {
    if(c == 0x0D || c == 0x0A) setupChess();
    return;
  }
  
  if(c == 0xB4) chessCursorX = (chessCursorX > 0) ? chessCursorX - 1 : 7;
  else if(c == 0xB7) chessCursorX = (chessCursorX < 7) ? chessCursorX + 1 : 0;
  else if(c == 0xB5) chessCursorY = (chessCursorY > 0) ? chessCursorY - 1 : 7;
  else if(c == 0xB6) chessCursorY = (chessCursorY < 7) ? chessCursorY + 1 : 0;
  else if(c == 0x0D || c == 0x0A) { // ENTER
    if(chessSelX == -1) {
      int p = chessBoard[chessCursorY][chessCursorX];
      if(p != 0 && ((p > 10 ? 1 : 0) == chessTurn)) {
        chessSelX = chessCursorX;
        chessSelY = chessCursorY;
      }
    } else {
      if(chessSelX == chessCursorX && chessSelY == chessCursorY) {
        chessSelX = -1; chessSelY = -1;
      } else {
        if(tryMove(chessSelY, chessSelX, chessCursorY, chessCursorX)) {
          chessBoard[chessCursorY][chessCursorX] = chessBoard[chessSelY][chessSelX];
          chessBoard[chessSelY][chessSelX] = 0;
          
          int movedPiece = chessBoard[chessCursorY][chessCursorX];
          if(movedPiece == 1 && chessCursorY == 0) chessBoard[chessCursorY][chessCursorX] = 5; 
          if(movedPiece == 11 && chessCursorY == 7) chessBoard[chessCursorY][chessCursorX] = 15; 
          
          chessTurn = 1 - chessTurn;
          chessSelX = -1; chessSelY = -1;
          
          if(!hasLegalMoves(chessTurn)) chessGameOver = true;
        } else {
          int p = chessBoard[chessCursorY][chessCursorX];
          if(p != 0 && ((p > 10 ? 1 : 0) == chessTurn)) {
            chessSelX = chessCursorX;
            chessSelY = chessCursorY;
          }
        }
      }
    }
  }
  drawChessBoard();
}

#endif