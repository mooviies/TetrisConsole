# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

**Linux (CLion):**
```
cmake -B cmake-build-debug
cmake --build cmake-build-debug
```
The binary is `cmake-build-debug/tetris`.

**Windows / Visual Studio:**
```
cmake -B build
cmake --build build
```
The binary is `build/TetrisConsole.exe`.

There are no tests or linting configured.

## IDE Setup

**CLion:** Open `CMakeLists.txt` directly — CLion handles CMake natively. The build directory is `cmake-build-debug/`.

**VS Code + clangd:** CMake generates `cmake-build-debug/compile_commands.json` automatically (`CMAKE_EXPORT_COMPILE_COMMANDS` is enabled). Point clangd at it or symlink:
```
ln -s cmake-build-debug/compile_commands.json .
```

**Windows / Visual Studio:** `cmake -B build` generates a `.sln` in `build/` — open it directly.

## Architecture

The project is a cross-platform console Tetris built in C++17, split into two layers. See `ARCHITECTURE.md` for full details.

### MVC Pattern

```
GameController (logic) --friend access--> GameState (model) <--const getters-- GameRenderer (view)
                                                ^
                                                |
                                 Tetris (facade) owns all three
```

- **GameController** — Pure game logic. Returns `StepResult` enum (`Continue`, `PauseRequested`, `GameOver`). Never touches renderer or menus.
- **GameState** — All game data (matrix, bag, score, level, highscores, flags). Members private; `friend class GameController` for write access, public `const` getters for views.
- **GameRenderer** — Owns display components: `ScoreDisplay`, `PieceDisplay` (next + hold), `PlayfieldDisplay`, `Icon` (mute). Calls `update()` then `render()` on each.
- **Tetris** — Facade that owns the above three plus `Menu&` references. Dispatches `StepResult`, handles pause/game-over flow, cycles music.

Dirty flag (`markDirty`/`isDirty`/`clearDirty`) on `GameState` controls when rendering happens.

### Platform Abstraction (`source/Konsole/`)

OS-specific code lives in `*Win32.cpp` / `*Linux.cpp` files. CMake uses `list(FILTER ... EXCLUDE)` to exclude the wrong platform's files at configure time — no `#ifdef` in game logic.

- **Platform.h** — Console init/cleanup/flush/getKey. Linux uses termios raw mode (VMIN=1, OPOST|ONLCR), terminal resize via ANSI `\033[8;rows;colst`, and a custom `getKey()` that does blocking reads with `select()`-based ANSI escape sequence parsing. Windows uses Console API and delegates `getKey()` to `rlutil::getkey()`.
- **Input.h** — Keyboard polling. Linux: non-blocking `select()` + `read()` from stdin parsing ANSI escape sequences. Windows: `GetKeyState()`. The game loop calls `Input::pollKeys()` once per frame, then queries individual key states via `left()`, `right()`, etc.
- **Panel** — Core UI rendering primitive. Box-drawn frame with rows of cells (text with alignment/color), separators with smart junctions, and `PanelElement` subclasses for custom content. Row-level dirty tracking for efficient redraws.
- **Menu** — Hierarchical menu system using `Platform::getKey()` for blocking input. Panel-based rendering. Supports option values (cycled with LEFT/RIGHT), callbacks, submenus, and per-option/per-value hint text.
- **SoundEngine** — Wraps miniaudio. Manages streamed music (A/B/C cycle) and decoded sound effects.
- **Timer** — Meyers singleton named-timer using `steady_clock`. Drives fall speed, autorepeat, and lock-down delays.
- **Random** — Static utility using `std::mt19937` with Meyers singleton pattern (static local generator).

### Game Logic (`source/Tetris/`)

- **TetrisConsole.cpp** — Entry point. Sets up menus (main, new game, options, pause, confirmations, game over), initializes subsystems, runs the main loop: `pollKeys() → step() → render()`.
- **Tetris** — Facade class (see MVC above). Owns `GameState`, `GameRenderer`, `GameController`, and `Menu&` references. Coordinates game flow, sound playback, and music cycling.
- **GameController** — Per-frame state machine (`GameStep`: Idle, MoveLeft, MoveRight, HardDrop). Handles gravity, SRS rotation, autorepeat (DAS), lock-down, line clearing, and scoring. Operates on `GameState` through friend access.
- **Tetrimino** — Live game piece. Holds position, rotation state, SRS wall kick data. `PieceData` provides static per-type data (color, preview strings, 4 facings, T-spin corner positions).
- **ScoreDisplay** — Panel showing Score, Time, TPM, LPM, Level, Lines, Tetris, Combos, T-Spins. Has `updateTimer()` for smooth TPM/LPM updates without full redraws.
- **HighScoreDisplay** — Standalone two-panel viewer (not a game HUD element). Left panel: ranked top-10 list. Right panel: detailed stats + game options for the selected entry. UP/DOWN navigation, ENTER/ESC to close.
- **PlayfieldDisplay** — 10x20 visible grid using a `PanelElement` subclass.
- **PieceDisplay** — Next queue (configurable 0-6 slots) and Hold piece preview panels.

### Dependencies (header-only, vendored in `include/`)

- **miniaudio.h** — Cross-platform audio (replaced FMOD). `#define MINIAUDIO_IMPLEMENTATION` is in SoundEngine.cpp. Vendored headers use `-isystem` to suppress warnings.
- **rlutil.h** — Console colors, cursor positioning, `getkey()`. Already cross-platform internally via `#ifdef _WIN32`. Extended with bright background ANSI codes (`\033[100m`–`\033[107m`) for colors 8-15 (DARKGREY through WHITE) in `getANSIBackgroundColor()`.

### Key Quirks

- `_shouldExit` is set via `Tetris::exit()` → `GameState::setShouldExit(true)`; the game exits from the main loop in `TetrisConsole.cpp`.
- Media files are embedded at build time via `scripts/embed_media.py` into `${CMAKE_BINARY_DIR}/media_data.{h,cpp}`. CMake runs this automatically when media files change.
- High scores persist as a flat top-10 leaderboard in `$XDG_DATA_HOME/TetrisConsole/score.bin` (Linux) or `%APPDATA%\TetrisConsole\score.bin` (Windows). Binary format v3, 80 bytes per record. Each record stores game stats and the options used (starting level, mode, ghost, hold, preview count).
- Game options persist separately in `options.bin` in the same data directory.
- Source files were originally encoded in CP437/CP1252 (Windows console). All box-drawing (`╔═║` etc.) and block characters (`██░░▒▒▄▀`) are now UTF-8.
- `Platform::flushOutput()` is called after drawing — necessary in termios raw mode where line-buffering is disabled.
- No `using namespace std;` in headers — all headers use explicit `std::` prefixes. `.cpp` files may use `using namespace std;`.
- Compiler warnings: `-Wall -Wextra -Wpedantic -Wshadow -Wconversion`. The build should be warning-free.
