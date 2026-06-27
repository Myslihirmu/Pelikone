#ifndef GAME_CHESS_H
#define GAME_CHESS_H
#include "Globals.h"
#include "Comms.h"

// 0 = Empty
// White: 1=P, 2=N, 3=B, 4=R, 5=Q, 6=K
// Black: 11=p, 12=n, 13=b, 14=r, 15=q, 16=k
int chessBoard[8][8];
int chessTurn = 0; // 0 = White, 1 = Black
int chessCursorX = 0, chessCursorY = 0;
int chessSelX = -1, chessSelY = -1;
bool chessGameOver = false;

// --- Phases / mode ---
#define CH_MENU    0
#define CH_PLAY    1
#define CH_PAIRING 2
#define CH_LOBBY   3
#define CH_DISCONN 4
#define CH_NET_STATE 1

int  chPhase = CH_MENU;
int  chMode  = 0;          // 0 = 2 players (one device), 1 = network, 2 = vs device (bot)
int  chMenuSel = 0;
int  chMyColor = 0;        // network: 0 white, 1 black
bool chOwnerIsWhite = true;
unsigned long chPairStart = 0;
int  chBotDepth = 2;       // vs device: search depth
int  chBotBlunder = 0;     // vs device: % chance of a random move (easy)

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

// --- Network-mode screens (board is full-screen, so these replace it) ---
void drawChessMenu() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("SHAKKI");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);
  const char* opt[4] = { "HELPPO", "VAIKEA", "2 PELAAJAA", "2 LAITETTA" };
  for (int i = 0; i < 4; i++) {
    int y = 44 + i * 22;
    if (i == chMenuSel) { tft.fillRect(0, y - 15, 128, 20, 0x18C3); u8g2_display.setForegroundColor(COLOR_GREEN); }
    else u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, y);
    u8g2_display.print(i == chMenuSel ? ">" : " ");
    u8g2_display.print(opt[i]);
  }
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 126); u8g2_display.print("ENTER = aloita");
}

void drawChessPairing() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("SHAKKI");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);
  u8g2_display.setForegroundColor(COLOR_TEXT);
  u8g2_display.setCursor(6, 58); u8g2_display.print("Etsitään toista");
  u8g2_display.setCursor(6, 78); u8g2_display.print("laitetta...");
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(6, 116); u8g2_display.print("ESC = peruuta");
}

void drawChessLobby() {
  tft.fillScreen(COLOR_BG);
  u8g2_display.setForegroundColor(COLOR_GRAY);
  u8g2_display.setCursor(2, 18); u8g2_display.print("SHAKKI");
  tft.drawFastHLine(0, 24, 128, COLOR_GRAY);
  if (commsLobbyOwner) {
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, 44); u8g2_display.print("Kumpi olet?");
    const char* opt[2] = { "VALKOINEN", "MUSTA" };
    for (int i = 0; i < 2; i++) {
      int y = 70 + i * 22;
      if (i == chMenuSel) { tft.fillRect(0, y - 15, 128, 20, 0x18C3); u8g2_display.setForegroundColor(COLOR_GREEN); }
      else u8g2_display.setForegroundColor(COLOR_TEXT);
      u8g2_display.setCursor(6, y);
      u8g2_display.print(i == chMenuSel ? ">" : " ");
      u8g2_display.print(opt[i]);
    }
    u8g2_display.setForegroundColor(COLOR_GRAY);
    u8g2_display.setCursor(2, 124); u8g2_display.print("Valkoinen aloittaa");
  } else {
    u8g2_display.setForegroundColor(COLOR_GREEN);
    u8g2_display.setCursor(6, 56); u8g2_display.print("Yhdistetty!");
    u8g2_display.setForegroundColor(COLOR_TEXT);
    u8g2_display.setCursor(6, 84); u8g2_display.print("Toinen valitsee");
    u8g2_display.setCursor(6, 104); u8g2_display.print("värin...");
  }
}

void drawChessDisconn() {
  tft.fillRect(14, 40, 100, 48, COLOR_POPUP_BG);
  tft.drawRect(14, 40, 100, 48, COLOR_BORDER);
  u8g2_display.setForegroundColor(COLOR_RED);
  if (commsPartnerLeft) { u8g2_display.setCursor(24, 60); u8g2_display.print("Vastustaja"); u8g2_display.setCursor(24, 78); u8g2_display.print("poistui"); }
  else                  { u8g2_display.setCursor(24, 60); u8g2_display.print("Yhteys"); u8g2_display.setCursor(24, 78); u8g2_display.print("katkesi"); }
}

void drawChessBoard() {
  if (chPhase == CH_MENU)    { drawChessMenu();    return; }
  if (chPhase == CH_PAIRING) { drawChessPairing(); return; }
  if (chPhase == CH_LOBBY)   { drawChessLobby();   return; }

  bool flip = (chMode == 1 && chMyColor == 1);   // black sees the board from its own side
  for(int r=0; r<8; r++) {
    for(int c=0; c<8; c++) {
      int sx = flip ? (7 - c) : c;
      int sy = flip ? (7 - r) : r;
      int x = sx * 16;
      int y = sy * 16;
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

  // Draw cursor (mapped into the flipped view for the black player)
  int curSX = flip ? (7 - chessCursorX) : chessCursorX;
  int curSY = flip ? (7 - chessCursorY) : chessCursorY;
  tft.drawRect(curSX * 16, curSY * 16, 16, 16, COLOR_YELLOW);
  tft.drawRect(curSX * 16 + 1, curSY * 16 + 1, 14, 14, COLOR_YELLOW);

  if (chPhase == CH_DISCONN) { drawChessDisconn(); return; }

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

// --- Chess bot (vs device): negamax + alpha-beta over pseudo-legal moves ---
int chPieceVal(int type) {
  switch (type) { case 1: return 100; case 2: return 320; case 3: return 330;
                  case 4: return 500; case 5: return 900; default: return 0; }
}

int chEvalWhite() {
  int s = 0;
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
    int p = chessBoard[r][c];
    if (!p) continue;
    int v = chPieceVal(p % 10);
    if (r >= 2 && r <= 5 && c >= 2 && c <= 5) v += 8;   // small centre bonus
    s += (p < 10) ? v : -v;
  }
  return s;
}

int chEvalFor(int color) { return color == 0 ? chEvalWhite() : -chEvalWhite(); }

int chApply(int sr, int sc, int dr, int dc, int& movedFrom) {
  int captured = chessBoard[dr][dc];
  movedFrom = chessBoard[sr][sc];
  chessBoard[dr][dc] = movedFrom;
  chessBoard[sr][sc] = 0;
  if (movedFrom == 1 && dr == 0) chessBoard[dr][dc] = 5;         // promote to queen
  else if (movedFrom == 11 && dr == 7) chessBoard[dr][dc] = 15;
  return captured;
}

void chUndo(int sr, int sc, int dr, int dc, int captured, int movedFrom) {
  chessBoard[sr][sc] = movedFrom;
  chessBoard[dr][dc] = captured;
}

int chNegamax(int depth, int alpha, int beta, int color) {
  if (depth == 0) return chEvalFor(color);
  int best = -2000000;
  for (int pass = 0; pass < 2; pass++)               // captures first, then quiet moves
    for (int sr = 0; sr < 8; sr++) for (int sc = 0; sc < 8; sc++) {
      int p = chessBoard[sr][sc];
      if (!p || ((p > 10 ? 1 : 0) != color)) continue;
      for (int dr = 0; dr < 8; dr++) for (int dc = 0; dc < 8; dc++) {
        if ((chessBoard[dr][dc] != 0) != (pass == 0)) continue;
        if (!isPseudoLegal(sr, sc, dr, dc)) continue;
        int mf, cap = chApply(sr, sc, dr, dc, mf);
        int score = (cap == 6 || cap == 16) ? (900000 + depth)   // king captured = win
                                            : -chNegamax(depth - 1, -beta, -alpha, 1 - color);
        chUndo(sr, sc, dr, dc, cap, mf);
        if (score > best) best = score;
        if (best > alpha) alpha = best;
        if (alpha >= beta) return best;
      }
    }
  return (best == -2000000) ? chEvalFor(color) : best;
}

// Bot is black (color 1): find and play its move.
void chBotMove() {
  if (chBotBlunder > 0 && (int)random(0, 100) < chBotBlunder) {  // easy: random legal move
    int count = 0;
    for (int sr = 0; sr < 8; sr++) for (int sc = 0; sc < 8; sc++) {
      if (chessBoard[sr][sc] < 11) continue;
      for (int dr = 0; dr < 8; dr++) for (int dc = 0; dc < 8; dc++) if (tryMove(sr, sc, dr, dc)) count++;
    }
    if (count == 0) return;
    int pick = random(0, count), idx = 0;
    for (int sr = 0; sr < 8; sr++) for (int sc = 0; sc < 8; sc++) {
      if (chessBoard[sr][sc] < 11) continue;
      for (int dr = 0; dr < 8; dr++) for (int dc = 0; dc < 8; dc++) {
        if (!tryMove(sr, sc, dr, dc)) continue;
        if (idx == pick) { int mf; chApply(sr, sc, dr, dc, mf); return; }
        idx++;
      }
    }
    return;
  }

  int bestScore = -2000000, bsr = -1, bsc = 0, bdr = 0, bdc = 0;
  for (int sr = 0; sr < 8; sr++) for (int sc = 0; sc < 8; sc++) {
    if (chessBoard[sr][sc] < 11) continue;                       // black pieces only
    for (int dr = 0; dr < 8; dr++) for (int dc = 0; dc < 8; dc++) {
      if (!tryMove(sr, sc, dr, dc)) continue;                    // legal root moves only
      int mf, cap = chApply(sr, sc, dr, dc, mf);
      int score = -chNegamax(chBotDepth - 1, -2000000, 2000000, 0);
      chUndo(sr, sc, dr, dc, cap, mf);
      if (bsr < 0 || score > bestScore) { bestScore = score; bsr = sr; bsc = sc; bdr = dr; bdc = dc; }
    }
    yield();   // keep the system responsive while thinking
  }
  if (bsr >= 0) { int mf; chApply(bsr, bsc, bdr, bdc, mf); }
}

// --- Network serialization (whole board each move) ---
void chSendState() {
  uint8_t b[68];
  b[0] = CH_NET_STATE;
  b[1] = chOwnerIsWhite ? 1 : 0;
  int k = 2;
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) b[k++] = (uint8_t)chessBoard[r][c];
  b[66] = (uint8_t)chessTurn;
  b[67] = chessGameOver ? 1 : 0;
  commsSend(b, 68);
}

void chRecvState(const uint8_t* b) {
  chOwnerIsWhite = (b[1] != 0);
  chMyColor = commsLobbyOwner ? (chOwnerIsWhite ? 0 : 1) : (chOwnerIsWhite ? 1 : 0);
  int k = 2;
  for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) chessBoard[r][c] = b[k++];
  chessTurn = b[66];
  chessGameOver = (b[67] != 0);
  chessSelX = -1; chessSelY = -1;
  if (chPhase == CH_LOBBY) { chessCursorX = 0; chessCursorY = 0; }
  chPhase = CH_PLAY;
  drawChessBoard();
}

// --- Game flow ---
void setupChess() {            // launched from main menu -> show the chooser
  chPhase = CH_MENU;
  chMenuSel = 0;
  drawChessBoard();
}

void chStartGame() {           // 2 players on one device
  initChessBoard();
  chessTurn = 0; chessCursorX = 0; chessCursorY = 0; chessSelX = -1; chessSelY = -1; chessGameOver = false;
  chPhase = CH_PLAY;
  drawChessBoard();
}

void chStartNetGame() {        // owner starts/restarts a network game
  initChessBoard();
  chessTurn = 0; chessCursorX = 0; chessCursorY = 0; chessSelX = -1; chessSelY = -1; chessGameOver = false;
  chPhase = CH_PLAY;
  drawChessBoard();
  chSendState();
}

void chessOnExit() {           // called by the .ino when leaving via ESC
  if (chMode == 1) { commsEnd(); chMode = 0; chPhase = CH_MENU; }
}

void updateChess() {           // called every loop; only does work in network mode
  if (chMode != 1) return;
  commsTick();

  if ((commsPartnerLeft || commsLinkLost) && (chPhase == CH_LOBBY || chPhase == CH_PLAY)) {
    chPhase = CH_DISCONN;
    drawChessBoard();
    return;
  }

  if (chPhase == CH_PAIRING) {
    if (commsPaired) { chPhase = CH_LOBBY; chMenuSel = 0; drawChessBoard(); }
    else if (millis() - chPairStart > 30000) { chessOnExit(); setupChess(); }
    return;
  }

  if (chPhase == CH_LOBBY || chPhase == CH_PLAY) {
    uint8_t buf[80]; uint8_t len;
    if (commsPoll(buf, &len) && len >= 68 && buf[0] == CH_NET_STATE) chRecvState(buf);
  }
}

void handleChessInput(char c) {
  if (chPhase == CH_MENU) {
    if      (c == 0xB5) { if (chMenuSel > 0) chMenuSel--; drawChessBoard(); }
    else if (c == 0xB6) { if (chMenuSel < 3) chMenuSel++; drawChessBoard(); }
    else if (c == 0x0D || c == 0x0A) {
      if      (chMenuSel == 0) { chMode = 2; chBotDepth = 2; chBotBlunder = 30; chStartGame(); }  // HELPPO
      else if (chMenuSel == 1) { chMode = 2; chBotDepth = 3; chBotBlunder = 0;  chStartGame(); }  // VAIKEA
      else if (chMenuSel == 2) { chMode = 0; chStartGame(); }                                     // 2 PELAAJAA
      else                     { chMode = 1; commsBegin(); chPhase = CH_PAIRING; chPairStart = millis(); drawChessBoard(); }  // 2 LAITETTA
    }
    return;
  }

  if (chPhase == CH_PAIRING || chPhase == CH_DISCONN) return;   // ESC (global) leaves

  if (chPhase == CH_LOBBY) {
    if (commsLobbyOwner) {
      if      (c == 0xB5) { chMenuSel = 0; drawChessBoard(); }
      else if (c == 0xB6) { chMenuSel = 1; drawChessBoard(); }
      else if (c == 0x0D || c == 0x0A) {
        chOwnerIsWhite = (chMenuSel == 0);
        chMyColor = chOwnerIsWhite ? 0 : 1;
        chStartNetGame();
      }
    }
    return;
  }

  // CH_PLAY
  if(chessGameOver) {
    if(c == 0x0D || c == 0x0A) {
      if (chMode == 1) { if (commsLobbyOwner) chStartNetGame(); }
      else chStartGame();
    }
    return;
  }

  if (chMode == 1 && chMyColor == 1) {   // black's board is flipped -> invert arrows
    if      (c == 0xB4) c = 0xB7;
    else if (c == 0xB7) c = 0xB4;
    else if (c == 0xB5) c = 0xB6;
    else if (c == 0xB6) c = 0xB5;
  }
  if(c == 0xB4) chessCursorX = (chessCursorX > 0) ? chessCursorX - 1 : 7;
  else if(c == 0xB7) chessCursorX = (chessCursorX < 7) ? chessCursorX + 1 : 0;
  else if(c == 0xB5) chessCursorY = (chessCursorY > 0) ? chessCursorY - 1 : 7;
  else if(c == 0xB6) chessCursorY = (chessCursorY < 7) ? chessCursorY + 1 : 0;
  else if(c == 0x0D || c == 0x0A) { // ENTER
    if (chMode == 1 && chessTurn != chMyColor) { drawChessBoard(); return; }  // not your turn
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
          if (chMode == 1) chSendState();
          if (chMode == 2 && !chessGameOver) {   // bot (black) replies
            chBotMove();
            chessTurn = 1 - chessTurn;
            if (!hasLegalMoves(chessTurn)) chessGameOver = true;
          }
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
