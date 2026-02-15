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

### Build Targets

Konsole is built as a **static library** (`libkonsole.a` / `konsole.lib`) that the Tetris executable links against. This enforces a clean dependency direction: Tetris depends on Konsole, never the reverse. The final output is still a single executable.

### Source Layout

```
TetrisConsole/source/
  Konsole/                  # Static library — platform abstraction & UI primitives
    Platform/               # OS abstraction (console, keyboard)
    UI/                     # Panel, Menu, Icon, Color, RowDrawContext
    Util/                   # Timer, Random, SoundEngine, Utility, Vector2i
  Tetris/                   # Executable — game logic
    Core/                   # MVC triad, facade, entry point, shared types
    Piece/                  # Tetrimino, PieceData, Facing (geometry & SRS)
    Rules/                  # Pluggable policies (ScoringRule, GravityPolicy, LockDownPolicy)
    Display/                # Panel-based display components (HUD & modals)
```

### MVC Pattern

```
GameController (orchestrator)
    |-- PieceMovement (falling phase: movement, rotation, DAS, lock-down)
    |-- LineClear (pattern/animate/eliminate phases: row detection, scoring)
    |
    | reads/writes via friend access
    v
GameState (model) ---const getters---> GameRenderer (view)
    ^                                       |
    |                                       | reads via public API
    +---------------------------------------+

Tetris (facade) owns all three, dispatches StepResult
```

- **GameController** — Orchestrator. Owns `PieceMovement` and `LineClear` helpers, dispatches game phases, handles generation/completion, bag management. Returns `StepResult` enum. Never touches renderer or menus.
- **PieceMovement** — Handles the Falling phase: gravity, DAS autorepeat, left/right/down movement, SRS rotation, hard drop, hold swap, lock-down. Transitions to Pattern phase on lock.
- **LineClear** — Handles Pattern/Iterate/Animate/Eliminate phases: full-row detection, flash animation, row elimination, scoring (delegates to `ScoringRule`), combo tracking.
- **GameState** — All game data (matrix, bag, score, level, highscores, flags). Members private; `friend class GameController` for write access, public `const` getters for views.
- **GameRenderer** — Owns display components: `ScoreDisplay`, `PieceDisplay` (next + hold), `PlayfieldDisplay`, `Icon` (mute). Calls `update()` then `render()` on each.
- **Tetris** — Facade that owns the above three plus `Menu&` references. Dispatches `StepResult`, handles pause/game-over flow, cycles music.

Dirty flag (`markDirty`/`isDirty`/`clearDirty`) on `GameState` controls when rendering happens.

### Konsole Layer (`source/Konsole/`)

OS-specific code lives in `*Win32.cpp` / `*Linux.cpp` files. CMake uses `list(FILTER ... EXCLUDE)` to exclude the wrong platform's files at configure time — no `#ifdef` in game logic.

- **Platform/** — `Platform.h` (console init/cleanup/flush/getKey), `Input.h`/`.cpp` (keyboard polling). Linux uses termios raw mode + `select()`; Windows uses Console API + `GetKeyState()`.
- **UI/** — `Panel` (box-drawn frame rendering primitive with row-level dirty tracking), `Menu` (hierarchical menu system), `Icon` (standalone indicator), `Color.h` (16 ANSI constants), `RowDrawContext` (element rendering helper).
- **Util/** — `Timer` (Meyers singleton named-timer), `Random` (mt19937 utility), `SoundEngine` (miniaudio wrapper with embedded VFS), `Utility`, `Vector2i`.

### Tetris Layer (`source/Tetris/`)

- **Core/** — `TetrisConsole.cpp` (entry point, main loop), `Tetris` (facade), `GameController` (phase orchestrator), `PieceMovement` (falling-phase logic), `LineClear` (line-clear pipeline), `GameState` (model), `GameRenderer` (view), `GameTypes.h`, `Constants.h`, `InputSnapshot.h`.
- **Piece/** — `Tetrimino` (live game piece), `PieceData` (static per-type data), `Facing` (rotation states + SRS wall kick data).
- **Rules/** — `ScoringRule` (Guideline scoring), `GravityPolicy` (speed table), `LockDownPolicy` (Extended/Infinite/Classic).
- **Display/** — `ScoreDisplay`, `PlayfieldDisplay`, `PieceDisplay`, `PiecePreview`, `HighScoreDisplay`, `HelpDisplay`.

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
