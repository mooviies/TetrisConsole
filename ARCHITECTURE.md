# Architecture

Cross-platform console Tetris in C++17. Two layers: a platform abstraction (`source/Konsole/`) and game logic (`source/Tetris/`). No third-party game frameworks — only vendored header-only libraries (`miniaudio.h`, `rlutil.h`).

---

## Table of Contents

1. [Main Loop & Tetris Facade](#1-main-loop--tetris-facade)
2. [MVC: GameController, GameState, GameRenderer](#2-mvc-gamecontroller-gamestate-gamerenderer)
3. [Piece Generation (Double-Bag Randomizer)](#3-piece-generation-double-bag-randomizer)
4. [Tetrimino, Facing & SRS Rotation](#4-tetrimino-facing--srs-rotation)
5. [Scoring, Leveling & Lock-Down](#5-scoring-leveling--lock-down)
6. [Input System](#6-input-system)
7. [Platform Abstraction](#7-platform-abstraction)
8. [Panel Rendering System](#8-panel-rendering-system)
9. [Sound Engine](#9-sound-engine)
10. [Timer](#10-timer)
11. [Random](#11-random)
12. [Menu System](#12-menu-system)
13. [Build System & Media Embedding](#13-build-system--media-embedding)

---

## 1. Main Loop & Tetris Facade

**Files:** `Tetris.h`, `Tetris.cpp`, `TetrisConsole.cpp`

### Entry Point

`main()` in `TetrisConsole.cpp` bootstraps the application:

1. `Platform::initConsole()` — terminal raw mode, alternate screen buffer, colors
2. `Input::init()` — input subsystem
3. `SoundEngine::init()` — miniaudio engine with embedded VFS
4. Wait for terminal to reach minimum size (80x29)
5. Build the menu tree (main, newGame, options, pause, quit, gameOver)
6. Create the `Tetris` facade, wire menu callbacks via lambdas
7. `main.open()` — blocking main menu
8. `tetris.start()` — initialize game state, begin music
9. Enter the main loop

### Main Loop

```
while (!tetris.doExit()) {
    Input::pollKeys()           // 1. gather input
    Platform::wasResized()?     // 2. handle terminal resize
    tetris.step()               // 3. game logic
    tetris.render()             // 4. draw (only if dirty)
}
```

### Tetris Facade

The `Tetris` class owns all core components and wires them together:

- `GameState _state` — model
- `GameRenderer _renderer` — view
- `GameController _controller` — pure logic
- `Menu& _pauseMenu`, `Menu& _gameOverMenu` — references to menu system

**`step()`** dispatches the controller's `StepResult`:

| Result | Action |
|--------|--------|
| `Continue` | Normal frame |
| `PauseRequested` | Pause music, render overlay, open pause menu |
| `GameOver` | Stop music, save highscore, open game over menu, reset |

After dispatching, `step()` also:
- Plays pending sounds queued by the controller
- Handles mute toggle requests
- Cycles music tracks (A -> B -> C -> A) when a track ends

**`render()`** checks `_state.isDirty()` before calling `_renderer.render()`, then clears the dirty flag. This avoids redundant redraws.

**`redraw()`** forces a full repaint — called on terminal resize.

### Exit Flow

The quit menu callback calls `tetris.exit()`, which sets `_state.setShouldExit(true)`. The main loop checks `tetris.doExit()` and exits cleanly, running `SoundEngine::cleanup()`, `Input::cleanup()`, and `Platform::cleanupConsole()`.

---

## 2. MVC: GameController, GameState, GameRenderer

**Files:** `GameController.h/.cpp`, `GameState.h/.cpp`, `GameRenderer.h/.cpp`

### Separation of Concerns

```
GameController (logic)
    |
    | reads/writes via friend access
    v
GameState (model) ---const getters---> GameRenderer (view)
    ^                                       |
    |                                       | reads via public API
    +---------------------------------------+

Tetris (facade) owns all three, dispatches StepResult
```

**GameController** never touches the renderer or menus. It operates on `GameState` through `friend class GameController` access to private members and returns a `StepResult` enum. The `Tetris` facade dispatches the result.

**GameState** holds all game data: matrix, bag, current/hold tetriminos, score, level, lines, flags. Members are private. The controller has friend access; the renderer reads through `const` public getters.

**GameRenderer** owns the display components (`ScoreDisplay`, `HighScoreDisplay`, `PieceDisplay` for next and hold, `PlayfieldDisplay`, `Icon` for mute). It calls `update()` on each display with data from `GameState`, then `render()` to draw.

### Dirty Flag

`GameState` has a dirty flag (`markDirty()` / `isDirty()` / `clearDirty()`). The controller calls `markDirty()` whenever state changes that affect the display (piece moved, lines cleared, score updated). The facade only calls `_renderer.render()` when dirty, then clears the flag.

### State Machine

The controller uses a `GameStep` enum for the per-frame state machine:

| State | Behavior |
|-------|----------|
| `Idle` | Spawn piece if needed, gravity fall, check left/right autorepeat, check hold |
| `MoveLeft` | Continue autorepeat left movement + fall |
| `MoveRight` | Continue autorepeat right movement + fall |
| `HardDrop` | Move piece down until collision, lock immediately |

---

## 3. Piece Generation (Double-Bag Randomizer)

**Files:** `GameState.h/.cpp`, `GameController.cpp`

### The Double-Bag (14 Objects)

The bag holds 14 `Tetrimino` objects — two copies of each of the 7 piece types (O, I, T, L, J, S, Z). They're logically split into two halves:

- **Indices 0-6**: the "current" bag (being consumed)
- **Indices 7-13**: the "next" bag (pre-shuffled, ready to go)

This exists because the next-piece preview needs to peek up to 4 pieces ahead. When `_bagIndex` is near the end (e.g. 5), peeking 4 ahead means reading indices 5, 6, 7, 8 — which crosses the single-bag boundary. The second half makes this safe. Worst case: `_bagIndex(6) + 4 = 10 < 14`.

### Shuffle (Fisher-Yates on a Subrange)

`GameController::shuffle(state, start)` runs Fisher-Yates over 7 elements starting at `start`:

```
for i = 6 down to 0:
    j = random integer in [0, i]
    swap bag[start + i] with bag[start + j]
```

It swaps `unique_ptr`s, not the `Tetrimino` objects themselves. Raw pointers like `_currentTetrimino` and `_holdTetrimino` (which point at the underlying objects) stay valid because `unique_ptr::swap` just exchanges ownership without moving objects in memory.

### Popping a Piece

`popTetrimino()`:

1. Grabs `_bag[_bagIndex]` as the new `_currentTetrimino`, increments `_bagIndex`.
2. When `_bagIndex` reaches 7 (first half exhausted):
   - `swap_ranges` swaps the two halves — the pre-shuffled "next" half becomes the new "current" half at indices 0-6.
   - Reshuffles indices 7-13 to prepare a fresh "next" bag.
   - Resets `_bagIndex` to 0.

This guarantees the Tetris Guideline's **7-bag randomizer**: every piece type appears exactly once per bag of 7, so you never go more than 12 pieces without seeing any given type.

### Feeding the Next Queue

`peekTetriminos(count)` reads `count` consecutive pieces starting at `_bagIndex` without consuming them. The renderer calls `peekTetriminos(NEXT_PIECE_QUEUE_SIZE)` (4) each frame and passes the vector to the `PieceDisplay`, which sets each preview slot directly.

### On Reset

`GameController::reset()` shuffles both halves independently and sets `_bagIndex = 0`, giving a fresh random sequence for each new game.

---

## 4. Tetrimino, Facing & SRS Rotation

**Files:** `Tetrimino.h/.cpp`, `Facing.h/.cpp`, `PieceData.h/.cpp`, `Vector2i.h`

### Piece Data (Static Table)

Each of the 7 piece types has a `PieceData` struct built once at startup:

| Field | Description |
|-------|-------------|
| `color` | ANSI color constant (e.g. `Color::LIGHTBLUE` for I) |
| `previewLine1/2` | Two-line UTF-8 preview string (e.g. `"  ▄▄▄▄▄▄▄▄  "`) |
| `startingPosition` | Default spawn: `{19, 4}` (row 19, column 4) |
| `facings[4]` | 4 rotation states (NORTH, EAST, SOUTH, WEST) |
| `hasTSpin` | `true` only for T-piece |
| `tSpinPositions[4]` | Corner positions for T-spin detection, one per facing |

`getPieceData(PieceType)` returns a const reference to the static table (Meyers singleton pattern — built on first call, never destroyed).

### Facing

A `Facing` represents one rotation state:

- `_minos`: vector of 4 `Vector2i` offsets relative to the piece's position
- `_direction`: which rotation (NORTH/EAST/SOUTH/WEST)
- `_rotationPoints[5]`: SRS wall kick test data

### Tetrimino

A `Tetrimino` is a live game piece. It holds:

- A reference to the `GameMatrix` (for collision checks)
- A copy of the piece data (color, facings, preview strings, t-spin data)
- Current position and rotation state
- Last rotation point used (for T-spin scoring)

Key methods:

| Method | Description |
|--------|-------------|
| `setPosition(pos)` | Place piece at position if valid |
| `move(delta)` | Move by delta if valid, updates position |
| `simulateMove(delta)` | Check if move would be valid without applying it |
| `rotate(direction)` | SRS rotation with 5 kick tests (see below) |
| `lock()` | Write mino colors into the matrix, return `true` if legal |
| `resetRotation()` | Reset to NORTH facing |
| `isMino(row, col)` | Check if a mino occupies this cell |

### SRS Rotation (Super Rotation System)

When `rotate(direction)` is called:

1. Compute the target rotation (current +/- 1, wrapping)
2. For each of the 5 rotation points in the current facing:
   - If the point doesn't exist (some facings skip point 4), skip it
   - Apply the kick offset for the given direction
   - Check if all 4 minos fit at the new position + offset
   - If valid: apply the rotation, record which kick test succeeded (1-5), return `true`
3. If all 5 tests fail, the rotation is rejected

The kick data is per-facing, per-direction, following the official SRS specification. Each `RotationPoint` stores two translation vectors — one for LEFT rotation, one for RIGHT.

### T-Spin Detection (3-Corner Rule)

Only applies to the T-piece. Each facing defines 4 corner positions (A, B, C, D) relative to the piece center:

- **A, B**: "front" corners (facing direction)
- **C, D**: "back" corners

**Full T-Spin**: A and B occupied, plus at least one of C or D occupied.

**Mini T-Spin**: C and D occupied, plus at least one of A or B occupied.

**Promotion rule**: A mini T-spin is promoted to a full T-spin if the rotation used SRS kick test 5 (the most extreme wall kick). This is tracked via `_lastRotationPoint`.

---

## 5. Scoring, Leveling & Lock-Down

**File:** `GameController.cpp` (primarily the `lock()` and `fall()` methods)

### Lock-Down Mechanics

When a piece can no longer move down, a lock-down timer starts (`LOCK_DOWN_DELAY = 0.5s`). During lock-down:

- Moving or rotating the piece resets the timer slightly (`LOCK_DOWN_SMALL_DELAY = 0.2s`) — this is the "Extended" lock-down mode
- A move counter tracks lock-down moves (max `LOCK_DOWN_MOVE = 15`)
- If the piece reaches a new lowest row during lock-down, the move counter resets and the timer restarts
- In `CLASSIC` mode, the small reset is disabled

When the timer expires or the move limit is reached, the piece locks: its mino colors are written into the matrix.

### Hard Drop

Moves the piece down until collision, scoring 2 points per cell dropped, then locks immediately. Soft drop scores 1 point per cell.

### Line Clearing & Scoring

After locking, full rows are detected and removed. Scoring follows the Tetris Guideline:

| Clear Type | Base Score | Awarded Lines |
|------------|-----------|---------------|
| Single | 100 | 1 |
| Double | 300 | 3 |
| Triple | 500 | 5 |
| Tetris (4 lines) | 800 | 8 |
| T-Spin Single | 400 | 8 |
| T-Spin Double | 800 | 12 |
| T-Spin Triple | 1200 | 16 |
| Mini T-Spin Single | 100 | 2 |

All base scores are multiplied by the current level. **Back-to-back bonus**: consecutive Tetrises or T-spin line clears get a 1.5x multiplier.

### Leveling

- `_goal` accumulates awarded lines
- When `_goal >= level * 5`, level increments and goal resets
- Gravity speed is looked up from `kSpeedNormal[level]` (or `kSpeedFast` during soft drop)
- Speeds are capped at level 15

### Autorepeat (DAS)

Left/right movement uses Delayed Auto Shift:

1. First press: move immediately, start timer
2. After `AUTOREPEAT_DELAY` (0.25s): enter autorepeat state
3. In autorepeat: move every `AUTOREPEAT_SPEED` (0.01s) — very fast repeat
4. Release: stop timer, return to Idle state

---

## 6. Input System

**Files:** `Input.h`, `InputLinux.cpp`, `InputWin32.cpp`

### Public Interface

```cpp
static void pollKeys();              // poll all pending keyboard events
static bool left();                  // query individual key states
static bool right();
static bool softDrop();
static bool hardDrop();
static bool rotateClockwise();
static bool rotateCounterClockwise();
static bool hold();
static bool pause();
static bool mute();
static bool select();                // menu enter/confirm
```

The game loop calls `pollKeys()` once per frame, then queries individual key states.

### Linux Implementation

**Polling model**: event-driven batch processing.

`pollKeys()` resets all 10 static bool flags to `false`, then drains all pending input from stdin using `::select()` with **zero timeout** (non-blocking). It reads up to 64 bytes per iteration and parses:

- **ANSI escape sequences** (`ESC [ A/B/C/D`): arrow keys
- **Single characters**: space (hard drop), z/x (rotate), c (hold), m (mute), escape (pause), enter (select)

Key queries return the cached static flags. `init()` and `cleanup()` are no-ops (terminal setup is handled by `Platform`).

### Windows Implementation

**Polling model**: direct state queries.

`pollKeys()` is an empty stub. Each key query method calls `GetKeyState(vKey) & 0x8000` directly, checking multiple virtual keys per action (e.g. `rotateClockwise()` checks `VK_UP`, `X`, and numpad 1/5/9). No caching — Windows reports current key state synchronously.

### Key Differences

| Aspect | Linux | Windows |
|--------|-------|---------|
| Polling | Batch drain + cache flags | No-op (lazy query) |
| Key query | Return cached flag | Direct `GetKeyState()` |
| Arrow keys | Manual ANSI escape parsing | OS-provided virtual keys |
| Numpad | Not supported | Full numpad support |

---

## 7. Platform Abstraction

**Files:** `Platform.h`, `PlatformLinux.cpp`, `PlatformWin32.cpp`

### Interface

```cpp
static void initConsole();
static void cleanupConsole();
static void flushInput();
static void flushOutput();
static int  getKey();              // blocking read (used by menus)
static bool wasResized();
static bool isTerminalTooSmall();
static void showResizePrompt();
static int  offsetX();             // centering offsets for content
static int  offsetY();
static void updateOffsets();
static std::string getDataDir();   // persistent data directory
```

### Linux

**Console init**: saves original `termios` settings, enters alternate screen buffer (`\033[?1049h`), sets raw mode with:
- `ICANON` off (no line buffering)
- `ECHO` off
- `ISIG` off (no Ctrl+C)
- `VMIN=1` (blocking reads for menus)
- `OPOST | ONLCR` on (so `\n` outputs as `\r\n`)

Registers a `SIGWINCH` handler to detect terminal resizes.

**`getKey()`**: blocking read from stdin with ANSI escape sequence parsing. Reads one byte; if it's `ESC`, uses `select()` with 50ms timeout to read the rest of the escape sequence. Maps arrow keys to rlutil constants. This is separate from `Input::pollKeys()` — `getKey()` is for menus (blocking), `pollKeys()` is for the game loop (non-blocking).

**Centering**: computes offsets to center the 80x29 game area in larger terminals: `offsetX = max(0, (cols - 80) / 2)`.

**Data directory**: `$XDG_DATA_HOME/TetrisConsole` or `~/.local/share/TetrisConsole`.

**Cleanup**: restores original terminal settings, shows cursor, leaves alternate screen buffer.

### Windows

**Console init**: disables Ctrl+C, centers window on desktop (675x515px), disables resize/maximize, hides cursor, sets 80x29 buffer, disables quick-edit mode.

**`getKey()`**: delegates to `rlutil::getkey()`.

**Resize**: all no-ops (fixed window size). Offsets always return 0.

**Data directory**: `%APPDATA%\TetrisConsole` via `SHGetFolderPathA()`.

---

## 8. Panel Rendering System

**Files:** `Panel.h/.cpp`, `RowDrawContext.h/.cpp`, `PiecePreview.h/.cpp`, `PieceDisplay.h/.cpp`, `PlayfieldDisplay.h/.cpp`, `ScoreDisplay.h/.cpp`, `HighScoreDisplay.h/.cpp`, `Icon.h/.cpp`, `Color.h`

### Color

16 ANSI color constants (0-15): BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, GREY, DARKGREY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE. Used throughout for foreground and background colors via `rlutil`.

### Panel

The core rendering primitive. A Panel is a box-drawn frame containing rows of content:

```
╔══════════════╗
║   Score      ║    <- TEXT row (centered)
╠══════════════╣    <- SEPARATOR with smart junctions
║  0000099999  ║    <- TEXT row
╠══════╦═══════╣    <- SEPARATOR adapts to column boundaries
║Level ║  01   ║    <- Multi-cell TEXT row
╚══════╩═══════╝
```

**Row types:**
- **TEXT**: one or more `Cell`s with text, alignment (LEFT/CENTER/RIGHT/FILL), color, and optional fixed width
- **SEPARATOR**: horizontal divider with smart junction characters (scans adjacent rows for column boundaries to place `╬`, `╦`, `╩`, or `═`)
- **ELEMENT**: delegates to a `PanelElement` subclass

**Width computation**: auto-measured from content, or fixed. Multi-cell rows distribute remaining space equally among auto-width cells after accounting for fixed-width cells and column separators (`║`).

**Dirty tracking**: row-level dirty flags (`_dirtyRows` vector) plus element-level `isDirty()`. `render()` only redraws dirty rows. `invalidate()` forces a full redraw.

### PanelElement (Virtual Interface)

```cpp
class PanelElement {
    virtual int height() const = 0;
    virtual void drawRow(int rowIndex, RowDrawContext& ctx) const = 0;
    bool isDirty() / markDirty() / clearDirty();
};
```

`RowDrawContext` provides `setColor()`, `setBackgroundColor()`, `print()`, and `width()` — elements render custom content without managing cursor positioning.

### PiecePreview

`PanelElement` subclass (height = 2). Stores two preview strings and a color. `setPiece(const Tetrimino*)` copies the piece's preview lines; `clearPiece()` blanks the display. Used by `PieceDisplay`.

### PieceDisplay

Composite display for the "Next" and "Hold" panels. Constructor takes a title and a size:
- Size 1 (Hold): title + separator + 1 preview slot
- Size N (Next): title + separator + 1 main slot + separator + (N-1) queue slots

`update(vector<const Tetrimino*>)` sets each slot directly from the vector — slot 0 gets the first piece, slot 1 the second, etc.

### PlayfieldDisplay

The 10x20 visible game field. Its `PlayfieldElement` (PanelElement subclass, height = 20) renders each row by checking:
- Is there a current tetrimino mino here? Draw `██` in piece color
- Is there a locked mino? Draw `░░` with the locked color as background
- Otherwise: draw a checkerboard pattern in dark grey (`░░` / `▒▒` alternating)

### ScoreDisplay

Panel (width 16) showing Score, Level, and Lines. Score color changes to green during back-to-back bonus. Values are zero-padded to fixed widths (10, 2, and 6 digits).

### HighScoreDisplay

Simpler panel (width 16) showing just the persistent high score, zero-padded to 10 digits.

### Icon

Standalone single-character display (not a PanelElement). Used for the mute indicator (`♪`). Changes color based on mute state: white (unmuted), yellow (music muted), red (all muted).

---

## 9. Sound Engine

**Files:** `SoundEngine.h/.cpp`

### Overview

Wraps miniaudio to provide music streaming and sound effect playback. All media is embedded in the binary — no external files at runtime.

### Embedded VFS (Virtual File System)

SoundEngine implements miniaudio's VFS callbacks to intercept file operations:

1. `embeddedVFS_onOpen("media/A.mp3")` calls `findEmbeddedMedia()` (generated by `embed_media.py`) to get a pointer to the compiled-in binary blob
2. Returns an `EmbeddedFile` handle with `{data, size, cursor}`
3. `onRead`, `onSeek`, `onTell`, `onInfo`, `onClose` implement a seekable stream over the blob
4. miniaudio's decoders (MP3, WAV) read from this virtual stream transparently

### Sound Types

| Type | Creation | Flag | Use |
|------|----------|------|-----|
| Music | `createStreamSound()` | `MA_SOUND_FLAG_STREAM` | Long tracks, streamed from embedded data |
| Effect | `createEffectSound()` | `MA_SOUND_FLAG_DECODE` | Short WAVs, fully decoded in memory |

All sounds are stored in a `map<string, MaSoundPtr>`. `MaSoundPtr` is a `unique_ptr` with a custom deleter that calls `ma_sound_uninit()`.

### Music Playback

5 music tracks: `A.mp3`, `B.mp3`, `C.mp3` (game music), `title.mp3`, `score.mp3` (menu music, looping).

`playMusic(name)` stops the current track, seeks to frame 0, sets volume, and starts. The `Tetris` facade checks `musicEnded()` each frame and cycles game music: A -> B -> C -> A.

### Mute State Machine

Three states, cycled by pressing M:

```
UNMUTED --> MUSIC_MUTED --> ALL_MUTED --> UNMUTED
              (save music vol,   (save effect vol,  (restore both)
               set to 0)          set to 0)
```

Saved volumes are restored when cycling back to UNMUTED.

---

## 10. Timer

**Files:** `Timer.h/.cpp`

### Design

Meyers singleton (`Timer::instance()`) managing named timers backed by `std::chrono::steady_clock`.

Internal storage: `map<string, time_point>` mapping timer IDs to their start timestamps.

### API

| Method | Description |
|--------|-------------|
| `startTimer(id)` | Record current time under `id` (overwrites if exists) |
| `resetTimer(id, seconds)` | Set timer to appear as if it started `seconds` ago |
| `stopTimer(id)` | Remove timer from the map |
| `getSeconds(id)` | Compute elapsed seconds since start; returns 0 if timer doesn't exist |
| `exist(id)` | Check if timer is active |

### Usage

The game uses named timers for:
- `"fall"` — gravity tick interval
- `"autorepeatleft"` / `"autorepeatright"` — DAS delay and repeat rate
- `"lockdown"` — 0.5s lock-down timer

Elapsed time is computed on-the-fly from `steady_clock::now() - start_time` — no cached values, no drift.

---

## 11. Random

**Files:** `Random.h/.cpp`

### Design

Static utility class (constructors deleted). Uses a `std::mt19937` Mersenne Twister generator as a Meyers singleton (static local in `getGenerator()`), seeded once from `std::random_device`.

### API

```cpp
static int getInteger(int min, int max);  // uniform random in [min, max] inclusive
```

Creates a fresh `uniform_int_distribution` for each call. Used by the bag shuffle (Fisher-Yates) to generate random swap indices.

---

## 12. Menu System

**Files:** `Menu.h/.cpp`

### Structure

A hierarchical, tree-based navigation system. Each `Menu` owns:

- `_options`: ordered list of item names
- `_menus`: submenu mappings (name -> `Menu*`)
- `_callbacks`: action callbacks (name -> `function<void(OptionChoice)>`)
- `_optionsValues` / `_optionsValuesChoices`: for items with selectable values (cycled with LEFT/RIGHT)
- `_closeOptions` / `_closeAllMenusOptions`: items that close the current menu or the entire menu tree

### Navigation

`open()` enters a blocking loop using `Platform::getKey()`:

| Key | Action |
|-----|--------|
| UP | Move selection up (wraps) |
| DOWN | Move selection down (wraps) |
| LEFT | Cycle option value backward |
| RIGHT | Cycle option value forward |
| ENTER | Select current item |
| ESCAPE | Close menu (if `escapeCloses` is true) |

Selection dispatches to:
1. **Submenu**: clear current menu, open submenu recursively
2. **Callback**: invoke with `OptionChoice` (selected index, all options, current values)
3. **Close option**: exit the loop

### Rendering

Delegates to a `Panel`. Selected item is prefixed with `"> "`, others with `"  "`. Items with values show `"Option : Value"`. Layout is auto-centered in the 80x29 area and regenerated on terminal resize.

### Integration

Two static function pointers allow external control:
- `Menu::shouldExitGame` — checked each loop iteration to break out when the game exits
- `Menu::onResize` — called on terminal resize to redraw surrounding UI

Menu callbacks use lambdas that capture the `Tetris` reference to set starting level, mode, or signal exit.

---

## 13. Build System & Media Embedding

**Files:** `CMakeLists.txt`, `scripts/embed_media.py`

### CMake Configuration

- C++17 standard, required
- `CMAKE_EXPORT_COMPILE_COMMANDS ON` for clangd
- Binary name: `tetris` (Linux/macOS), `TetrisConsole.exe` (Windows)

### Platform Source Exclusion

```cmake
if(WIN32)
    list(FILTER KONSOLE_SRCS EXCLUDE REGEX "Linux\\.cpp$")
else()
    list(FILTER KONSOLE_SRCS EXCLUDE REGEX "Win32\\.cpp$")
endif()
```

No `#ifdef` in game logic — the wrong platform's files are excluded at configure time.

### Compiler Flags

- **GCC/Clang**: `-Wall -Wextra -Wpedantic -Wshadow -Wconversion`
- **MSVC**: `/W3 /sdl`

Vendored headers in `TetrisConsole/include/` use `-isystem` to suppress their warnings.

### Platform Linking

| Platform | Libraries |
|----------|-----------|
| Windows | `winmm`, `ole32` |
| macOS | `pthread`, `dl`, `m`, CoreAudio, AudioToolbox, CoreFoundation |
| Linux | `pthread`, `dl`, `m` |

### Media Embedding Pipeline

`scripts/embed_media.py` runs at build time via `add_custom_command`, triggered when any file in `TetrisConsole/media/` changes:

1. Scans `TetrisConsole/media/` for all files
2. Generates `${CMAKE_BINARY_DIR}/media_data.h` — declares `findEmbeddedMedia()`
3. Generates `${CMAKE_BINARY_DIR}/media_data.cpp`:
   - One `static const unsigned char media_X[]` array per file (hex bytes)
   - A lookup table (`EmbeddedMediaEntry`) mapping `"media/filename"` to data pointer and size
   - A `findEmbeddedMedia()` function that searches the table by string comparison

**Embedded media files** (10 total):

| File | Type | Usage |
|------|------|-------|
| `A.mp3`, `B.mp3`, `C.mp3` | Music | Game music (cycled A->B->C) |
| `title.mp3` | Music | Title screen (looping) |
| `score.mp3` | Music | High score screen (looping) |
| `click.wav` | Effect | Piece move/rotate |
| `lock.wav` | Effect | Piece lock |
| `harddrop.wav` | Effect | Hard drop |
| `lineclear.wav` | Effect | 1-3 line clear |
| `tetris.wav` | Effect | 4-line clear (Tetris) |
