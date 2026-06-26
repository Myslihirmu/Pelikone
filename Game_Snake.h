#ifndef GAME_SNAKE_H
#define GAME_SNAKE_H
#include "Globals.h"

#define GRID_SIZE 8
#define BOARD_WIDTH 16  
#define BOARD_HEIGHT 14 

struct Point { int x, y; };
Point snake[100]; 
int snakeLen = 3; Point food;
int dirX = 1, dirY = 0; int nextDirX = 1, nextDirY = 0;
unsigned long lastMoveTime = 0; int moveInterval = 200;
int snakeScore = 0; int snakeHighScore = 0; bool snakeGameOver = false;

void spawnFood() {
  // FIX: retry until food lands on a cell not occupied by the snake
  do {
    food.x = random(0, BOARD_WIDTH);
    food.y = random(0, BOARD_HEIGHT);
    bool onSnake = false;
    for (int i = 0; i < snakeLen; i++) {
      if (food.x == snake[i].x && food.y == snake[i].y) { onSnake = true; break; }
    }
    if (!onSnake) break;
  } while (true);
}
void initSnake() { Preferences p; p.begin("snake", true); snakeHighScore = p.getInt("highscore", 0); p.end(); }

// Cell + HUD primitives shared by the full redraw and the incremental update.
#define SNAKE_BODY 0x0500
void snakeDrawCell(int gx, int gy, uint16_t color) {
  tft.fillRect(gx * GRID_SIZE, (gy * GRID_SIZE) + 16, GRID_SIZE-1, GRID_SIZE-1, color);
}
void snakeDrawHud() {
  tft.fillRect(0, 0, 128, 14, COLOR_BG);
  tft.drawFastHLine(0, 14, 128, COLOR_BORDER);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(5, 12); u8g2_display.print("Pisteet: "); u8g2_display.print(snakeScore);
  u8g2_display.setCursor(80, 12); u8g2_display.print("HI: "); u8g2_display.print(snakeHighScore);
}

// Full-scene redraw — used by refreshDisplay() (e.g. after the battery popup) and on game over.
void drawSnake() {
  tft.fillScreen(COLOR_BG);
  snakeDrawHud();
  snakeDrawCell(food.x, food.y, 0xF800);
  for(int i=0; i<snakeLen; i++) snakeDrawCell(snake[i].x, snake[i].y, (i == 0) ? COLOR_GREEN : SNAKE_BODY);
  if(snakeGameOver) {
    tft.fillRect(5, 40, 118, 70, COLOR_BG); tft.drawRect(5, 40, 118, 70, COLOR_GRAY);
    u8g2_display.setCursor(25, 65); u8g2_display.setForegroundColor(COLOR_GREEN); u8g2_display.print("PELI OHI!");
    u8g2_display.setCursor(25, 95); u8g2_display.setForegroundColor(COLOR_TEXT); u8g2_display.print("Paina Enter");
  }
}

void setupSnake() {
  snakeLen = 3; snakeScore = 0; dirX = 1; dirY = 0; nextDirX = 1; nextDirY = 0;
  snakeGameOver = false; moveInterval = 200;
  for(int i=0; i<snakeLen; i++) snake[i] = {5 - i, 7};
  spawnFood(); drawSnake();
}

void handleSnakeInput(char c) {
  if (snakeGameOver && (c == 0x0D || c == 0x0A)) setupSnake();
  if (c == 0xB5 && dirY == 0) { nextDirX = 0; nextDirY = -1; } 
  if (c == 0xB6 && dirY == 0) { nextDirX = 0; nextDirY = 1; }  
  if (c == 0xB4 && dirX == 0) { nextDirX = -1; nextDirY = 0; } 
  if (c == 0xB7 && dirX == 0) { nextDirX = 1; nextDirY = 0; }  
}

void updateSnake() {
  if (snakeGameOver || millis() - lastMoveTime < moveInterval) return;
  lastMoveTime = millis(); dirX = nextDirX; dirY = nextDirY;
  Point nextHead = {snake[0].x + dirX, snake[0].y + dirY};
  if (nextHead.x < 0 || nextHead.x >= BOARD_WIDTH || nextHead.y < 0 || nextHead.y >= BOARD_HEIGHT) snakeGameOver = true;
  // FIX: exclude tail (index snakeLen-1) — it vacates its cell this same tick when not eating
  for(int i=0; i<snakeLen-1; i++) if(nextHead.x == snake[i].x && nextHead.y == snake[i].y) snakeGameOver = true;
  if (snakeGameOver) { drawSnake(); return; }

  // Incremental draw: recolor only the cells that change this tick instead of
  // repainting the whole board (flicker-free, like Breakout/RalliF1).
  Point oldHead = snake[0];
  Point oldTail = snake[snakeLen - 1];
  bool ate = (nextHead.x == food.x && nextHead.y == food.y);

  if (ate) {
    snakeLen++; snakeScore++;
    if(snakeScore > snakeHighScore) { snakeHighScore = snakeScore; Preferences p; p.begin("snake", false); p.putInt("highscore", snakeHighScore); p.end(); }
    if(moveInterval > 70) moveInterval -= 5; spawnFood();
  }
  for (int i = snakeLen - 1; i > 0; i--) snake[i] = snake[i - 1];
  snake[0] = nextHead;

  snakeDrawCell(oldHead.x, oldHead.y, SNAKE_BODY);          // former head becomes body
  if (!ate) snakeDrawCell(oldTail.x, oldTail.y, COLOR_BG);  // erase the vacated tail
  snakeDrawCell(snake[0].x, snake[0].y, COLOR_GREEN);       // new head (drawn last)
  if (ate) { snakeDrawCell(food.x, food.y, 0xF800); snakeDrawHud(); }  // new food + score
}
#endif