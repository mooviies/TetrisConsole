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
    Rules/                  # Pluggable policies (ScoringRule, GravityPolicy, LockDownPolicy, GoalPolicy, VariantRule)
    Display/                # Panel-based display components (HUD, modals, Confetti, HelpDisplay)
    Test/                   # Debug-only test runner (TETRIS_DEBUG)
```

### MVC Pattern

```
GameController (orchestrator)
    |-- PieceMovement (falling phase: movement, rotation, DAS, lock-down)
    |-- LineClear (pattern/animate/eliminate phases: row detection, scoring)
    |
    | reads/writes public sub-structs
    v
GameState (model) ---const getters---> GameRenderer (view)
    ^                                       |
    |                                       | reads via public API
    +---------------------------------------+

Tetris (facade) owns all three, dispatches StepResult
```

- **GameController** — Orchestrator. Owns `PieceMovement`, `LineClear`, and five pluggable policy objects (`LockDownPolicy`, `ScoringRule`, `GravityPolicy`, `GoalPolicy`, `VariantRule`). Dispatches game phases, handles generation/completion, bag management. `configureVariant()` sets variant-specific policies and game config. Returns `StepResult` enum. Never touches renderer or menus.
- **PieceMovement** — Handles the Falling phase: gravity, DAS autorepeat, left/right/down movement, SRS rotation, hard drop, hold swap, lock-down. Transitions to Pattern phase on lock.
- **LineClear** — Handles Pattern/Iterate/Animate/Eliminate phases: full-row detection, flash animation, row elimination, scoring (delegates to `ScoringRule`), combo tracking.
- **GameState** — All game data via public sub-structs: `GameConfig` (variant, mode, ghost, hold, preview, timeLimit, showGoal), `Stats` (score, level, lines, goal, combos, etc.), `LockDownState`, `PieceState` (bag, current, hold), `FrameFlags`, `GameMatrix`, `LineClearState`. High scores are per-variant (`HighScoreTable = std::array<std::vector<HighScoreRecord>, VARIANT_COUNT>`). Private members: dirty flag, sound queue, game timer, player name.
- **GameRenderer** — Owns display components: `ScoreDisplay`, `PieceDisplay` (next + hold), `PlayfieldDisplay`, `Icon` (mute). Has `configure(previewCount, holdEnabled, showGoal)` to adjust the UI. Calls `update()` then `render()` on each.
- **Tetris** — Facade that owns the above three plus `Menu&` references and `HighScoreDisplay&`. Dispatches `StepResult`, handles pause/game-over flow, cycles music. `step()` takes an `InputSnapshot` parameter.

Dirty flag (`markDirty`/`isDirty`/`clearDirty`) on `GameState` controls when rendering happens.

### Konsole Layer (`source/Konsole/`)

OS-specific code lives in `*Win32.cpp` / `*Linux.cpp` files. CMake uses `list(FILTER ... EXCLUDE)` to exclude the wrong platform's files at configure time — no `#ifdef` in game logic.

- **Platform/** — `Platform.h` (console init/cleanup/flush/getKey), `Input.h`/`.cpp` (action-based key binding system). `Input` provides `init(actionCount)`, `bind(action, key)`, `clearBindings(action)`, `action(int)`, `getBindings(action)`, `keyName(key)`. Uses `KeyCode` enum for all key identifiers (ASCII + special keys like arrows, F-keys, numpad). Linux resolves KeyCodes from raw bytes via `select()` + ANSI escape parsing; Windows queries `GetKeyState()` for all bound keys per action.
- **UI/** — `Panel` (box-drawn frame rendering primitive with row-level dirty tracking), `Menu` (hierarchical menu system), `Icon` (standalone indicator), `Color.h` (16 ANSI constants), `RowDrawContext` (element rendering helper).
- **Util/** — `Timer` (Meyers singleton named-timer), `Random` (mt19937 utility), `SoundEngine` (miniaudio wrapper with embedded VFS), `Utility`, `Vector2i`.

### Tetris Layer (`source/Tetris/`)

- **Core/** — `TetrisConsole.cpp` (entry point, main loop), `Tetris` (facade), `GameController` (phase orchestrator), `PieceMovement` (falling-phase logic), `LineClear` (line-clear pipeline), `GameState` (model), `GameRenderer` (view), `GameTypes.h`, `Constants.h` (includes `VARIANT` enum, `VARIANT_COUNT`), `InputSnapshot.h` (defines `Action` enum and `InputSnapshot` struct).
- **Piece/** — `Tetrimino` (live game piece), `PieceData` (static per-type data), `Facing` (rotation states + SRS wall kick data).
- **Rules/** — `ScoringRule` (Guideline scoring), `GravityPolicy` (speed table), `LockDownPolicy` (Extended/Infinite/Classic), `GoalPolicy` (VariableGoal for Marathon, FixedGoal for Sprint), `VariantRule` (MarathonVariant, SprintVariant, UltraVariant).
- **Display/** — `ScoreDisplay`, `PlayfieldDisplay`, `PieceDisplay`, `PiecePreview`, `HighScoreDisplay` (per-variant tabs, confetti), `HelpDisplay` (two-panel key bindings), `Confetti` (particle animation).
- **Test/** — `TestRunner` (debug-only scenario-based regression tests, guarded by `TETRIS_DEBUG`).

### Dependencies (header-only, vendored in `include/`)

- **miniaudio.h** — Cross-platform audio (replaced FMOD). `#define MINIAUDIO_IMPLEMENTATION` is in SoundEngine.cpp. Vendored headers use `-isystem` to suppress warnings.
- **rlutil.h** — Console colors, cursor positioning, `getkey()`. Already cross-platform internally via `#ifdef _WIN32`. Extended with bright background ANSI codes (`\033[100m`–`\033[107m`) for colors 8-15 (DARKGREY through WHITE) in `getANSIBackgroundColor()`.

### Key Quirks

- `_shouldExit` is set via `Tetris::exit()` → `GameState::setShouldExit(true)`; the game exits from the main loop in `TetrisConsole.cpp`.
- Media files are embedded at build time via `scripts/embed_media.py` into `${CMAKE_BINARY_DIR}/media_data.{h,cpp}`. CMake runs this automatically when media files change.
- High scores are per-variant: `HighScoreTable = std::array<std::vector<HighScoreRecord>, VARIANT_COUNT>` (3 variants × top 10 each). Stored in `score.bin` in `$XDG_DATA_HOME/TetrisConsole/` (Linux) or `%APPDATA%\TetrisConsole\` (Windows).
- Game options persist separately in `options.bin` in the same data directory.
- `TETRIS_DEBUG` compile definition is set for Debug builds (`$<$<CONFIG:Debug>:TETRIS_DEBUG>`), enabling the test runner menu item.
- CMakeLists.txt has duplicate source entries for `GoalPolicy.{cpp,h}` and `VariantRule.{cpp,h}` (listed explicitly in `add_executable` in addition to `GLOB_RECURSE`). This is harmless but should be cleaned up.
- Source files were originally encoded in CP437/CP1252 (Windows console). All box-drawing (`╔═║` etc.) and block characters (`██░░▒▒▄▀`) are now UTF-8.
- `Platform::flushOutput()` is called after drawing — necessary in termios raw mode where line-buffering is disabled.
- No `using namespace std;` in headers — all headers use explicit `std::` prefixes. `.cpp` files may use `using namespace std;`.
- Compiler warnings: `-Wall -Wextra -Wpedantic -Wshadow -Wconversion`. The build should be warning-free.
