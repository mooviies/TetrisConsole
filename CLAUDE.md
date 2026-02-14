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

The project is a cross-platform console Tetris built in C++17, split into two layers:

### Platform Abstraction (`source/Konsole/`)

OS-specific code lives in `*Win32.cpp` / `*Linux.cpp` files. CMake uses `list(FILTER ... EXCLUDE)` to exclude the wrong platform's files at configure time — no `#ifdef` in game logic.

- **Platform.h** — Console init/cleanup/flush/getKey. Linux uses termios raw mode (VMIN=1, OPOST|ONLCR), terminal resize via ANSI `\033[8;rows;colst`, and a custom `getKey()` that does blocking reads with `select()`-based ANSI escape sequence parsing. Windows uses Console API and delegates `getKey()` to `rlutil::getkey()`.
- **Input.h** — Keyboard polling. Linux: non-blocking `select()` + `read()` from stdin parsing ANSI escape sequences. Windows: `GetKeyState()`. The game loop calls `Input::pollKeys()` once per frame, then queries individual key states via `left()`, `right()`, etc.
- **SoundEngine** — Wraps miniaudio. Manages streamed music (A/B/C cycle) and decoded sound effects.
- **Menu** — Self-contained hierarchical menu system using `Platform::getKey()` for blocking navigation input.
- **Timer** — Meyers singleton named-timer using `clock()`. Drives fall speed, autorepeat, and lock-down delays.
- **Random** — Static utility using `std::mt19937` with Meyers singleton pattern (static local generator).

### Game Logic (`source/Tetris/`)

- **TetrisConsole.cpp** — Entry point. Sets up menus, initializes subsystems, runs the main loop: `pollKeys() → step() → SoundEngine::update()`.
- **Tetris** — Core game class. Uses a function-pointer state machine (`_stepState`) for states: idle, moving left/right, hard dropping. Manages the 40x10 matrix (rows 0-19 are the invisible buffer zone above the playfield, 20-39 are visible). Owns tetriminos via `std::unique_ptr`. Speed arrays are `constexpr std::array`. Score uses `int64_t`.
- **Tetrimino** — Base class for all 7 piece types (I/J/L/O/S/T/Z). Each has 4 `Facing` objects (stored in `std::array<Facing, 4>`) defining rotation states with SRS-style wall kick data (`std::array<RotationPoint, 5>`).
- **Overseer** — Global accessor singleton bridging menu callbacks to the active Tetris instance (asserts non-null).

### Dependencies (header-only, vendored in `include/`)

- **miniaudio.h** — Cross-platform audio (replaced FMOD). `#define MINIAUDIO_IMPLEMENTATION` is in SoundEngine.cpp. Vendored headers use `-isystem` to suppress warnings.
- **rlutil.h** — Console colors, cursor positioning, `getkey()`. Already cross-platform internally via `#ifdef _WIN32`. Extended with bright background ANSI codes (`\033[100m`–`\033[107m`) for colors 8-15 (DARKGREY through WHITE) in `getANSIBackgroundColor()`.

### Key Quirks

- `Tetris::_shouldExit` is set via `Tetris::exit()`; the game exits from the main loop in `TetrisConsole.cpp`.
- Media files are embedded at build time via `scripts/embed_media.py` into `${CMAKE_BINARY_DIR}/media_data.{h,cpp}`. CMake runs this automatically when media files change.
- The `Menu::save()` method body is empty — it previously used Windows-only `ReadConsoleOutput` and was dead code.
- High score persists in `$XDG_DATA_HOME/TetrisConsole/score.bin` (Linux) or `%APPDATA%\TetrisConsole\score.bin` (Windows).
- Source files were originally encoded in CP437/CP1252 (Windows console). All box-drawing (`╔═║` etc.) and block characters (`██░░▒▒▄▀`) are now UTF-8.
- `Tetris::refresh()` flushes stdout after drawing — necessary in termios raw mode where line-buffering is disabled.
- No `using namespace std;` in headers — all headers use explicit `std::` prefixes. `.cpp` files may use `using namespace std;`.
- Compiler warnings: `-Wall -Wextra -Wpedantic -Wshadow -Wconversion`. The build should be warning-free.
