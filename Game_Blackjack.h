#ifndef GAME_BLACKJACK_H
#define GAME_BLACKJACK_H
#include "Globals.h"

int bjPlayer[10], bjPlayerCount;
int bjDealer[10], bjDealerCount;
bool bjGameOver;
int bjResult; // 0=none, 1=player win, 2=dealer win, 3=push, 4=bust
int bjStreak = 0;

String getCardStr(int c) {
  if (c == 1) return "A";
  if (c == 11) return "J";
  if (c == 12) return "Q";
  if (c == 13) return "K";
  return String(c);
}

int calcScore(int hand[], int count) {
  int sum = 0, aces = 0;
  for (int i = 0; i < count; i++) {
    int v = hand[i];
    if (v == 1) { aces++; sum += 11; }
    else if (v > 10) sum += 10;
    else sum += v;
  }
  while (sum > 21 && aces > 0) { sum -= 10; aces--; }
  return sum;
}

void initBlackjack() {
  Preferences p; 
  p.begin("bj", true); 
  bjStreak = p.getInt("streak", 0); 
  p.end();
}

void drawCardObj(int x, int y, int card, bool hidden) {
  tft.fillRect(x, y, 24, 34, hidden ? COLOR_GRAY : COLOR_TEXT);
  tft.drawRect(x, y, 24, 34, COLOR_BORDER);
  if (!hidden) {
    u8g2_display.setForegroundColor(0x0000);
    // Adjust single vs double digit centering slightly
    int offset = (card == 10) ? 1 : 5; 
    u8g2_display.setCursor(x + offset, y + 23);
    u8g2_display.print(getCardStr(card));
  }
}

void drawBlackjackBoard() {
  tft.fillScreen(COLOR_BG);
  
  // Dealer
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(5, 15);
  u8g2_display.print("Jakaja");
  if (bjGameOver) {
    u8g2_display.print(" ("); 
    u8g2_display.print(calcScore(bjDealer, bjDealerCount)); 
    u8g2_display.print(")");
  }

  // Dealer cards (hide second card if playing)
  for (int i = 0; i < bjDealerCount; i++) {
    drawCardObj(5 + i * 26, 20, bjDealer[i], (!bjGameOver && i == 1));
  }

  // Player
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(5, 75);
  u8g2_display.print("Pelaaja ("); 
  u8g2_display.print(calcScore(bjPlayer, bjPlayerCount)); 
  u8g2_display.print(")");

  // Player cards
  for (int i = 0; i < bjPlayerCount; i++) {
    drawCardObj(5 + i * 26, 80, bjPlayer[i], false);
  }

  // Game over popup or instructions
  if (bjGameOver) {
    tft.fillRect(10, 45, 108, 30, COLOR_POPUP_BG);
    tft.drawRect(10, 45, 108, 30, COLOR_BORDER);
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(18, 67);
    if (bjResult == 1) { u8g2_display.setForegroundColor(COLOR_GREEN); u8g2_display.print("VOITTO!"); }
    else if (bjResult == 2) { u8g2_display.print("HÄVISIT"); }
    else if (bjResult == 3) { u8g2_display.setForegroundColor(COLOR_YELLOW); u8g2_display.print("TASAPELI"); }
    else if (bjResult == 4) { u8g2_display.print("YLI 21!"); }
  } else {
    u8g2_display.setForegroundColor(COLOR_YELLOW);
    u8g2_display.setCursor(5, 125);
    u8g2_display.print("H=Ota  S=Jää"); 
  }
}

void endDealerTurn() {
  bjGameOver = true;
  while (calcScore(bjDealer, bjDealerCount) < 17 && bjDealerCount < 10) {
    bjDealer[bjDealerCount++] = random(1, 14);
  }
  
  int pScore = calcScore(bjPlayer, bjPlayerCount);
  int dScore = calcScore(bjDealer, bjDealerCount);
  
  if (dScore > 21 || pScore > dScore) {
    bjResult = 1; bjStreak++;
  } else if (pScore < dScore) {
    bjResult = 2; bjStreak = 0;
  } else {
    bjResult = 3;
  }
  
  Preferences p; p.begin("bj", false); p.putInt("streak", bjStreak); p.end();
}

void checkBust() {
  if (calcScore(bjPlayer, bjPlayerCount) > 21) {
    bjGameOver = true; bjResult = 4; bjStreak = 0;
    Preferences p; p.begin("bj", false); p.putInt("streak", bjStreak); p.end();
  }
}

void setupBlackjack() {
  bjGameOver = false; bjResult = 0;
  bjPlayerCount = 2; bjDealerCount = 2;
  bjPlayer[0] = random(1, 14); bjPlayer[1] = random(1, 14);
  bjDealer[0] = random(1, 14); bjDealer[1] = random(1, 14);
  
  if (calcScore(bjPlayer, bjPlayerCount) == 21) endDealerTurn();
  drawBlackjackBoard();
}

void handleBlackjackInput(char c) {
  if (bjGameOver) {
    if (c == 0x0D || c == 0x0A) setupBlackjack();
    return;
  }
  
  if (c == 'h' || c == 'H') {
    if (bjPlayerCount < 10) bjPlayer[bjPlayerCount++] = random(1, 14);
    checkBust();
    drawBlackjackBoard();
  } else if (c == 's' || c == 'S') {
    endDealerTurn();
    drawBlackjackBoard();
  }
}
#endif