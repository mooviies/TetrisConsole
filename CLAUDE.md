# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

**Linux/macOS:**
```
make            # build → ./tetris (lowercase, avoids conflict with TetrisConsole/ dir)
make clean      # remove build artifacts, binary, and media symlink
```
The Makefile auto-detects the OS via `uname -s` and selects platform-specific source files.

**Windows:** Open `TetrisConsole.sln` in Visual Studio (v143 toolset, C++17).

There are no tests or linting configured.

## Architecture

The project is a cross-platform console Tetris built in C++17, split into two layers:

### Platform Abstraction (`source/Konsole/`)

OS-specific code lives in `*Win32.cpp` / `*Linux.cpp` files. The Makefile uses `filter-out` to exclude the wrong platform's files at compile time — no `#ifdef` in game logic.

- **Platform.h** — Console init/cleanup/flush. Linux uses termios raw mode; Windows uses Console API.
- **Input.h** — Keyboard polling. Linux: non-blocking `read()` from stdin parsing ANSI escape sequences. Windows: `GetKeyState()`. The game loop calls `Input::pollKeys()` once per frame, then queries individual key states via `left()`, `right()`, etc.
- **SoundEngine** — Wraps miniaudio. Manages streamed music (A/B/C cycle) and decoded sound effects.
- **Menu** — Self-contained hierarchical menu system using rlutil's blocking `getkey()` for navigation.

### Game Logic (`source/Tetris/`)

- **TetrisConsole.cpp** — Entry point. Sets up menus, initializes subsystems, runs the main loop: `pollKeys() → step() → SoundEngine::update()`.
- **Tetris** — Core game class. Uses a function-pointer state machine (`_stepState`) for states: idle, moving left/right, hard dropping. Manages the 40x10 matrix (rows 0-19 are the invisible buffer zone above the playfield, 20-39 are visible).
- **Tetrimino** — Base class for all 7 piece types (I/J/L/O/S/T/Z). Each has 4 `Facing` objects defining rotation states with SRS-style wall kick data.
- **Overseer** — Global accessor singleton bridging menu callbacks to the active Tetris instance.
- **Timer** — Named-timer singleton using `clock()`. Drives fall speed, autorepeat, and lock-down delays.

### Dependencies (header-only, vendored in `include/`)

- **miniaudio.h** — Cross-platform audio (replaced FMOD). `#define MINIAUDIO_IMPLEMENTATION` is in SoundEngine.cpp.
- **rlutil.h** — Console colors, cursor positioning, `getkey()`. Already cross-platform internally via `#ifdef _WIN32`.

### Key Quirks

- `Tetris::_exit` is never set to true; the game exits via `exit(0)` in `quitGame()`.
- Media files live in `TetrisConsole/media/`; the Makefile creates a `media/` symlink at the project root so relative paths work.
- The `Menu::save()` method body is empty — it previously used Windows-only `ReadConsoleOutput` and was dead code.
- `score.bin` at the project root persists the high score between sessions.
