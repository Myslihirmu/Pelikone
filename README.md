# Pelikone

A small handheld game console built on an **ESP32-C6**, with a 128×128 colour OLED, an I²C keypad, and a rechargeable battery. 17 games, Finnish UI.

## Hardware
- **ESP32-C6** (WiFi 6 / Bluetooth LE / 802.15.4), 4 MB flash
- **SSD1351** 128×128 SPI OLED
- I²C keypad (CardKB-style, address `0x5F`)
- LiPo battery (1350 mAh) + power switch

## Games
Sanuli · Matopeli · 2048 · Ristinolla · Miinaharava · Flappy Bird · Shakki · Blackjack · Sokoban · Tetris · GeoDash · Ralli F1 · Tähtää · Hirsipuu · Pac-Man · Tiiliseinä · Neljän suora

## Controls (everywhere)
Arrows move/steer · **ENTER** select/confirm · **ESC** back to menu · **TAB** battery status. In word games, **Ä = `.`**, **Ö = `,`**.

## Build & flash
Built with the **Arduino IDE** (ESP32 core). Open `Pelikone_V8.ino`, select the ESP32-C6 board, and upload.
Libraries: Adafruit GFX, Adafruit SSD1351, U8g2_for_Adafruit_GFX.

> Coming soon: one-click **web flasher** + auto-built binaries via GitHub Actions, so updates can be flashed from a browser with no toolchain.

## Repo layout
- `Pelikone_V8.ino`, `Game_*.h`, `Globals.h`, `Menu.h` — firmware
- `tools/gen_manual.py` — generates the Finnish user manual PDF
- `tools/gen_sanuli_words.py` + `tools/*.txt` — generates the Sanuli word arrays
- `Pelikone_Kayttoohje.pdf` — the Finnish user manual

## Adding a game
1. Create `Game_X.h` with `initX / setupX / drawX / handleXInput` (+ `updateX` for real-time games).
2. Wire it into `Globals.h` (`NUM_GAMES` + the `Mode` enum), `Menu.h` (menu name), and `Pelikone_V8.ino` (include, `refreshDisplay`, `init`, menu launch, input dispatch).
3. Add a row to the `games` list in `tools/gen_manual.py`, then run `python3 tools/gen_manual.py` to update the manual.
