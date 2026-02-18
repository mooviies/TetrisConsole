# Architecture

Cross-platform console Tetrominos game in C++17. Two layers: a platform abstraction library (KonsoleGE, a git submodule from `mooviies/KonsoleGE`, built as a static library) and game logic (`source/Game/`, the executable that links against it). No third-party game frameworks — only vendored header-only libraries (`miniaudio.h`, `rlutil.h`) in `KonsoleGE/include/`.

---

## Table of Contents

1. [Main Loop & Tetrominos Facade](#1-main-loop--tetrominos-facade)
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

## 1. Main Loop & Tetrominos Facade

**Files:** `KonsoleGE/source/Core/GameEngine.h/.cpp`, `Game/Core/TetrominosGame.h/.cpp`, `Game/Core/TetrominosConsole.cpp`, `Game/Core/Tetrominos.h/.cpp`, `Game/Core/GameMenus.h/.cpp`

### Entry Point

`main()` in `TetrominosConsole.cpp` is a minimal wrapper — it creates a `TetrominosGame` and calls `run()`:

```cpp
int main() {
    TetrominosGame game;
    return game.run();
}
```

### GameEngine (Base Class)

`GameEngine` (`KonsoleGE/source/Core/`) provides a generic game loop with virtual callbacks:

```cpp
class GameEngine {
    virtual void onInit() = 0;
    virtual void onFrame(double dt) = 0;
    virtual void onCleanup() {}
    virtual void onResize() {}
    virtual void onTerminalTooSmall() {}
    virtual void onTerminalRestored() {}
    void setTargetFps(int fps);   // default 60
    void requestExit(int exitCode = 0);
};
```

`run()` handles the lifecycle: `Platform::initConsole()`, wait for minimum terminal size (80x29), call `onInit()`, then enter the frame loop. After the loop exits, it calls `onCleanup()` then `Platform::cleanupConsole()`.

### TetrominosGame (Game Driver)

`TetrominosGame` inherits from `GameEngine` and owns all top-level components:

- `std::unique_ptr<GameMenus> _menus` — menu tree and callbacks
- `std::unique_ptr<HighScoreDisplay> _highScores` — high score viewer
- `std::unique_ptr<HelpDisplay> _help` — key bindings viewer
- `std::unique_ptr<Tetrominos> _game` — game facade
- `Screen _screen` — state machine (`MainMenu` or `Playing`)

**`onInit()`** bootstraps the application:

1. `Input::init(actionCount)` — initialize action-based input system with `Action::Count` (9) actions
2. Bind default keys to actions (arrows, WASD, numpad, etc.) via `Input::bind(action, key)`
3. `SoundEngine::init()` — miniaudio engine with embedded VFS
4. Render title banner via `GameRenderer::renderTitle()`
5. Create `GameMenus`, `HighScoreDisplay`, `HelpDisplay`, and `Tetrominos` facade
6. `GameMenus::configure()` — wire all menu callbacks

**`onFrame()`** implements a screen state machine:

- **MainMenu**: opens the blocking main menu. On return, calls `game.start()` and transitions to Playing.
- **Playing**: polls input, calls `game.step(snapshot)` and `game.render()`. If `backToMenu()`, transitions back to MainMenu.

**`onResize()`** calls `game.redraw()`. **`onTerminalTooSmall()`** pauses the game timer. **`onTerminalRestored()`** resumes it.

### GameMenus

`GameMenus` (`Game/Core/`) owns and configures the entire menu tree:

- `_main`, `_newGame`, `_options` — main menu hierarchy
- `_pause`, `_pauseSound` — pause menu with sound submenu
- `_restartConfirm`, `_backToMenuConfirm`, `_quit` — confirmation dialogs
- `_gameOver` — game over menu

`configure(game, highScores, help)` wires all menu callbacks using lambdas that capture the `Tetrominos` reference. Sound settings (Music volume, Effects volume, Soundtrack mode) are managed via `syncSoundToMenu()` / `applySoundFromMenu()` helper methods that translate between `SoundEngine` state and menu option values.

### Tetrominos Facade

The `Tetrominos` class owns the MVC components and wires them together:

- `GameState _state` — model
- `GameRenderer _renderer` — view
- `GameController _controller` — pure logic
- `Menu& _pauseMenu`, `Menu& _gameOverMenu` — references to menu system
- `HighScoreDisplay& _highScoreDisplay` — reference to high score viewer (for new-entry prompts)

**`step(snapshot)`** takes an `InputSnapshot` and dispatches the controller's `StepResult`:

| Result | Action |
|--------|--------|
| `Continue` | Normal frame |
| `PauseRequested` | Pause timer, pause music, render overlay (playfield hidden), open pause menu |
| `GameOver` | Pause timer, stop music, prompt player name if new high score, save highscore, open game over menu |

After dispatching, `step()` also:
- Plays pending sounds queued by the controller (`GameSound` enum: Click, Lock, HardDrop, LineClear, Quad)
- Advances music tracks when the current track ends, respecting the active `SoundtrackMode` (Cycle: A→B→C→A, Random: random different track, TrackA/B/C: loop the chosen track)

**`render()`** checks `_state.isDirty()` before calling `_renderer.render()`, then clears the dirty flag. When not dirty, it calls `_renderer.renderTimer()` to update only the time, TPM, and LPM displays (smooth per-frame updates without full redraws).

**`redraw()`** forces a full repaint — called on terminal resize.

### Pause Flow

When `StepResult::PauseRequested` is returned:
1. Pause game timer and music (`SoundEngine::pauseMusic()`)
2. Render playfield hidden (replaced with blank)
3. Open pause menu (Resume / Restart / Options / Main Menu / Exit Game)
4. On Resume: invalidate renderer, unpause music (or switch track if soundtrack mode changed during pause), resume timer
5. On Restart: stop music, reset controller/state, reconfigure renderer, restart music
6. On Main Menu: stop music, set `_backToMenu` flag

### Game Over Flow

When `StepResult::GameOver` is returned:
1. Pause game timer, stop music
2. If new high score: prompt player name (Panel-based text input, max 10 chars)
3. Save highscore to disk
4. Open game over menu (Retry / Main Menu / Exit Game) — shows "New High Score!" subtitle if applicable
5. On Retry: reset and restart
6. On Main Menu: set `_backToMenu` flag

### Exit Flow

The quit menu callback calls `game.exit()`, which sets `_state.setShouldExit(true)`. `TetrominosGame::onFrame()` checks `game.doExit()` and calls `requestExit()`, which causes `GameEngine::run()` to exit the frame loop. Cleanup runs in order: `onCleanup()` (destroys components, `SoundEngine::cleanup()`, `Input::cleanup()`), then `Platform::cleanupConsole()`.

---

## 2. MVC: GameController, GameState, GameRenderer

**Files:** `Game/Core/GameController.h/.cpp`, `Game/Core/PieceMovement.h/.cpp`, `Game/Core/LineClear.h/.cpp`, `Game/Core/GameState.h/.cpp`, `Game/Core/GameRenderer.h/.cpp`

### Separation of Concerns

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

Tetrominos (facade) owns all three, dispatches StepResult
```

**GameController** is the orchestrator. It owns `PieceMovement`, `LineClear`, and five pluggable policy objects (`LockDownPolicy`, `ScoringRule`, `GravityPolicy`, `GoalPolicy`, `VariantRule`). It dispatches game phases, handles Generation/Completion phases directly, manages the double-bag randomizer, and coordinates setup/reset. `configureVariant(variant, state)` sets variant-specific policies (goal, leveling, time limit) and updates `GameConfig`. Never touches the renderer or menus. Returns a `StepResult` enum; the `Tetrominos` facade dispatches the result.

**PieceMovement** handles everything during `GamePhase::Falling`: gravity, DAS autorepeat, left/right/down movement, SRS rotation, hard drop, hold swap, and lock-down. When a piece locks, it transitions to `GamePhase::Pattern`. Takes non-owning pointers to `LockDownPolicy` and `GravityPolicy` (owned by GameController).

**LineClear** handles the Pattern → Iterate → Animate → Eliminate cycle: full-row detection, flash animation, row elimination, and scoring (delegates to `ScoringRule`). Takes a non-owning pointer to `ScoringRule`.

**GameState** holds all game data via public sub-structs:

- `GameConfig config` — variant, lock-down mode, ghost, hold, preview count, starting level, time limit, showGoal
- `Stats stats` — score, level, lines, goal, quad count, combos, T-spins, back-to-back, highscore threshold
- `LockDownState lockDown` — active flag, move count, lowest line
- `PieceState pieces` — bag (14 `unique_ptr<Tetrimino>`), bag index, current/hold pointers, isNewHold
- `FrameFlags flags` — step state, rotation/T-spin flags, game-over/started flags
- `GameMatrix matrix` — the 40-row deque
- `LineClearState lineClear` — cleared rows, flash state, notification/combo text
- `HardDropTrail hardDropTrail` — trail animation state (columns, row range, fade progress)

High scores are per-variant: `HighScoreTable = std::array<std::vector<HighScoreRecord>, VARIANT_COUNT>`. Private members include the dirty flag, sound queue, game timer, and player name.

**GameRenderer** owns the display components (`ScoreDisplay`, `PieceDisplay` for next and hold, `PlayfieldDisplay`). It calls `update()` on each display with data from `GameState`, then `render()` to draw. Has `configure(previewCount, holdEnabled, showGoal)` to adjust the UI based on game options (showGoal controls whether the goal/lines-remaining display appears in `ScoreDisplay`), `renderTimer()` to update only the time/TPM/LPM without full redraws, and a static `renderTitle(subtitle)` method that draws a centered title banner. `render()` takes an optional `playfieldVisible` parameter (default `true`) — set to `false` during pause to hide the playfield. At levels above 10, `render()` also draws a side notification overlay showing line-clear and combo text over the bottom of the next-piece queue panel.

### Dirty Flag

`GameState` has a dirty flag (`markDirty()` / `isDirty()` / `clearDirty()`). The controller calls `markDirty()` whenever state changes that affect the display (piece moved, lines cleared, score updated). The facade only calls `_renderer.render()` when dirty, then clears the flag.

### Game Phases

The controller dispatches based on `GamePhase`:

| Phase | Handler | Description |
|-------|---------|-------------|
| `Generation` | GameController | Wait for generation delay, pop piece from bag, spawn |
| `Falling` | PieceMovement | Player input, gravity, movement, rotation, lock-down |
| `Pattern` | LineClear | Detect full rows, queue sounds |
| `Iterate` | GameController | Immediate transition to Animate |
| `Animate` | LineClear | Flash animation on cleared rows |
| `Eliminate` | LineClear | Remove rows, award score, check cascades |
| `Completion` | GameController | Level up if needed, start generation timer |

### Movement State Machine

Within the Falling phase, `PieceMovement` uses a `GameStep` enum:

| State | Behavior |
|-------|----------|
| `Idle` | Gravity fall, check left/right autorepeat, check hold |
| `MoveLeft` | Continue autorepeat left movement + fall |
| `MoveRight` | Continue autorepeat right movement + fall |
| `HardDrop` | Move piece down until collision, lock immediately |

---

## 3. Piece Generation (Double-Bag Randomizer)

**Files:** `Game/Core/GameState.h/.cpp`, `Game/Core/GameController.cpp`

### The Double-Bag (14 Objects)

The bag holds 14 `Tetrimino` objects — two copies of each of the 7 piece types (O, I, T, L, J, S, Z). They're logically split into two halves:

- **Indices 0-6**: the "current" bag (being consumed)
- **Indices 7-13**: the "next" bag (pre-shuffled, ready to go)

This exists because the next-piece preview needs to peek up to 6 pieces ahead (default preview count). When `_bagIndex` is near the end (e.g. 5), peeking 6 ahead means reading indices 5, 6, 7, 8, 9, 10 — which crosses the single-bag boundary. The second half makes this safe. Worst case: `_bagIndex(6) + 6 = 12 < 14`.

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

This guarantees the Guideline's **7-bag randomizer**: every piece type appears exactly once per bag of 7, so you never go more than 12 pieces without seeing any given type.

### Feeding the Next Queue

`peekTetriminos(count)` reads `count` consecutive pieces starting at `_bagIndex` without consuming them. The renderer calls `peekTetriminos(previewCount)` (configurable, default 6) each frame and passes the vector to the `PieceDisplay`, which sets each preview slot directly.

### On Reset

`GameController::reset()` shuffles both halves independently and sets `_bagIndex = 0`, giving a fresh random sequence for each new game.

---

## 4. Tetrimino, Facing & SRS Rotation

**Files:** `Game/Piece/Tetrimino.h/.cpp`, `Game/Piece/Facing.h/.cpp`, `Game/Piece/PieceData.h/.cpp`, `KonsoleGE/source/Util/Vector2i.h`

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

**Files:** `Game/Core/PieceMovement.cpp` (`fall()`, `lock()`), `Game/Core/LineClear.cpp` (`awardScore()`, `detectFullRows()`, `eliminateRows()`), `Game/Rules/ScoringRule.h/.cpp`, `Game/Rules/LockDownPolicy.h/.cpp`, `Game/Rules/GravityPolicy.h/.cpp`, `Game/Rules/GoalPolicy.h/.cpp`, `Game/Rules/VariantRule.h/.cpp`

### Lock-Down Mechanics

When a piece can no longer move down, a lock-down timer starts (`LOCK_DOWN_DELAY = 0.5s`). Three modes govern what happens during lock-down:

| Mode | Timer reset on move/rotate | Move limit |
|------|---------------------------|------------|
| **Extended** | Full reset to 0.5s | 15 moves (`LOCK_DOWN_MOVE`), then forced lock |
| **Infinite** | Full reset to 0.5s | No limit |
| **Classic** | No reset | No limit (timer always runs to expiry) |

In Extended and Infinite modes, every successful move or rotation during lock-down calls `resetLockDown()`, which resets the timer to a fresh 0.5s. This lets the player keep manipulating the piece as long as they keep acting within each 0.5s window. Extended caps this at 15 moves to prevent infinite stalling; Infinite has no such cap.

In Classic mode, `resetLockDown()` returns immediately — the 0.5s timer runs uninterrupted from the moment the piece touches down.

In all modes, if the piece reaches a new lowest row during gravity fall while in lock-down, the move counter resets to 0 and the timer restarts (this is separate from `resetLockDown` — it happens inside `fall()`).

When the timer expires or (in Extended mode only) the move limit is reached, `lock()` is called.

### The `lock()` Method (in PieceMovement)

`lock()` orchestrates the transition from an active piece to a committed piece in the matrix. After locking, the game transitions to `GamePhase::Pattern` where `LineClear` takes over for line detection and scoring:

```
lock()
│
├─ Guard: no piece or game already over → return
│
├─ False alarm check: can piece still move down?
│  YES → Piece was nudged off its resting surface
│         (e.g. slid over a gap). Cancel the lock:
│         stop timer, reset move counter, update
│         lowestLine → return
│
├─ Commit: Tetrimino::lock() writes minos into matrix
│  FAILED → Lock-out (piece in buffer zone)
│           → isGameOver = true → return
│
├─ Reset lock-down state
│  (clear flags, stop timer, null out currentTetrimino)
│
└─ phase = Pattern, stepState = Idle, markDirty
```

### Line Clear Pipeline (in LineClear)

After lock, the game enters a multi-phase pipeline handled by `LineClear`:

1. **Pattern** (`stepPattern`): scans visible rows from bottom to top. If full rows found, queues sound and transitions to Iterate/Animate. If none, runs `awardScore(0)` for combo reset and transitions to Completion.
2. **Animate** (`stepAnimate`): flashes cleared rows on/off for 0.4s.
3. **Eliminate** (`stepEliminate`): erases full rows from the deque, pushes empty rows at front to maintain 40-row height, awards score, checks for cascading clears.

### Hard Drop

Moves the piece down until collision, scoring 2 points per cell dropped, then calls `lock()` immediately. Soft drop scores 1 point per cell.

Before dropping, `PieceMovement::fall()` snapshots the piece's position, color, and occupied columns into `HardDropTrail`. After dropping, if the piece actually moved, the trail is activated and a `"harddroptrail"` timer starts. `GameController::step()` linearly advances `visibleStartRow` toward `endRow` over 150ms, creating a top-to-bottom fade. `PlayfieldDisplay` renders trail cells as `░░` in the piece's color (only in otherwise-empty cells — locked blocks, the active piece, and the ghost take priority).

### `awardScore()`

Handles all scoring, stats, leveling, and sound queuing after a lock. Three branches based on the last move:

**T-Spin clears** (`_lastMoveIsTSpin`):

| Lines | Base Score | Awarded Lines |
|-------|-----------|---------------|
| 1 | 400 | 8 |
| 2 | 800 | 12 |
| 3 | 1200 | 16 |
| 0 (no clear) | 0 | 4 |

**Mini T-Spin clears** (`_lastMoveIsMiniTSpin`):

| Lines | Base Score | Awarded Lines |
|-------|-----------|---------------|
| 1 | 100 | 2 |
| 0 (no clear) | 0 | 1 |

**Normal clears**:

| Lines | Base Score | Awarded Lines | Back-to-Back |
|-------|-----------|---------------|--------------|
| 1 (Single) | 100 | 1 | Breaks streak |
| 2 (Double) | 300 | 3 | Breaks streak |
| 3 (Triple) | 500 | 5 | Breaks streak |
| 4 (Quad) | 800 | 8 | Enables streak |

All base scores are multiplied by the current level. **Back-to-back bonus**: consecutive Quads or T-spin line clears get a 1.5x score multiplier. Singles, Doubles, and Triples break the streak.

After scoring, `awardScore()` advances leveling, tracks stats, and queues sounds:

- `_lines += awardedLines`, `_goal += awardedLines`
- When `_goal >= level * 5` → level up, goal resets
- Combo tracking: `_currentCombo` counts consecutive line-clearing locks (-1 = no chain). `_combos` tracks the best combo achieved.
- `_quad` counts 4-line clears, `_tSpins` counts T-spin line clears
- `_nbMinos` increments per piece locked (feeds TPM calculation: `_nbMinos / minutesElapsed`)
- 4 lines → Quad sound; 1-3 lines → LineClear sound

### Game Variants

Three game modes are supported, configured via `VariantRule` and `GoalPolicy`:

| Variant | Goal | Level Up | Time Limit | GoalPolicy |
|---------|------|----------|------------|------------|
| **Marathon** | Score as high as possible | Yes (level × 5 lines per level) | None | `VariableGoal` |
| **Sprint** | Clear 40 lines | No (fixed starting level) | None | `FixedGoal` (40 lines total) |
| **Ultra** | Highest score in 2 min | No (fixed starting level) | 120 seconds | `VariableGoal` |

`VariantRule` is an abstract interface with `linesGoal()`, `levelUp()`, `timeLimit()`, and `showGoal()`. Concrete subclasses: `MarathonVariant`, `SprintVariant`, `UltraVariant`.

`GoalPolicy` is an abstract interface governing how level-up goals work: `goalValue(level)` returns lines needed for a level, `startingGoalValue(level)` handles non-level-1 starts, and `useAwardedLines()` controls whether awarded lines (from T-spins, etc.) count. `VariableGoal` (Marathon/Ultra) scales per level; `FixedGoal` (Sprint) uses a flat 40-line total.

`GameController::configureVariant()` instantiates the appropriate `VariantRule` and `GoalPolicy`, and writes the derived settings (`timeLimit`, `showGoal`) into `GameState::config`.

### Gravity

Gravity speed is looked up from `kSpeedNormal[level]` (or `kSpeedFast` during soft drop). Speeds are capped at level 15.

### Autorepeat (DAS)

Left/right movement uses Delayed Auto Shift:

1. First press: move immediately, start timer
2. After `AUTOREPEAT_DELAY` (0.25s): enter autorepeat state
3. In autorepeat: move every `AUTOREPEAT_SPEED` (0.01s) — very fast repeat
4. Release: stop timer, return to Idle state

### High Score Persistence

**File:** `Game/Core/GameState.cpp` (persistence) + `Game/Core/GameState.h` (`HighScoreRecord` struct)

Per-variant top-10 leaderboards stored as a binary file (`score.bin`). `HighScoreTable` is `std::array<std::vector<HighScoreRecord>, VARIANT_COUNT>` — one sorted vector per variant (Marathon, Sprint, Ultra). Each record stores both game stats and the options used during that game:

```
Header: magic(4) + version(4) + count(4) = 12 bytes

Per record (80 bytes):
Offset  Size  Field
0       8     score (int64)
8       4     level reached (int32)
12      4     lines (int32)
16      4     tpm (int32)
20      4     lpm (int32)
24      4     quad count (int32)
28      4     combos (int32)
32      4     tSpins (int32)
36      8     gameElapsed (double, seconds)
44      16    name (null-padded string)
60      4     startingLevel (int32)
64      4     mode (int32: 0=Extended, 1=Infinite, 2=Classic)
68      4     ghostEnabled (int32, 0/1)
72      4     holdEnabled (int32, 0/1)
76      4     previewCount (int32)
```

Magic: `0x53484354` ("TCHS" little-endian), version 3.

**New high score detection**: `activateHighscore()` sets the threshold to the 10th-place score (or 0 if fewer than 10 entries). Any score exceeding this threshold triggers the "New High Score!" flow: player name prompt → insert into sorted list → truncate to 10 → save.

**Options persistence**: game settings (starting level, mode, ghost, hold, preview) are stored separately in `options.bin` (magic `0x54434F50`, version 1, five int32 values).

### Game Timer

`GameState` owns a `steady_clock`-based game timer (`startGameTimer`, `pauseGameTimer`, `resumeGameTimer`, `gameElapsed`). It accumulates elapsed time across pause/resume cycles and is used for Time display, TPM, and LPM calculations.

---

## 6. Input System

**Files:** `KonsoleGE/source/Platform/Input.h`, `KonsoleGE/source/Platform/Input.cpp`, `KonsoleGE/source/Platform/InputLinux.cpp`, `KonsoleGE/source/Platform/InputWin32.cpp`, `Game/Core/InputSnapshot.h`

### Architecture

The input system uses an **action-based binding model**. Rather than hardcoding key-to-action mappings, the caller declares how many actions exist, then binds one or more `KeyCode` values to each action. At runtime, `pollKeys()` determines which keys are pressed and sets the corresponding action flags.

### KeyCode Enum (in `Input.h`)

Identifies all supported keys:

- **Printable ASCII** (0–255): stored as raw char codes (uppercase letters `'A'`–`'Z'`, digits `'0'`–`'9'`, `' '`)
- **Special keys** (256+): `ArrowUp`, `ArrowDown`, `ArrowLeft`, `ArrowRight`, `Enter`, `Escape`, `Tab`, `Backspace`, `Shift`, `Control`, `F1`–`F12`, `Insert`, `Delete`, `Home`, `End`, `PageUp`, `PageDown`, `Numpad0`–`Numpad9`, `NumpadDel`

### Action Enum (in `InputSnapshot.h`)

Defines the game's logical actions:

```
Left, Right, SoftDrop, HardDrop, RotateCW, RotateCCW, Hold, Pause, Select, Count
```

`InputSnapshot` is a plain struct with a `bool` field for each action (except Select, which is menu-only). It is populated each frame by `TetrominosGame::pollInputSnapshot()` from `Input::action()` queries and passed to `Tetrominos::step()`.

### Public Interface

```cpp
static void init(int actionCount);                    // allocate binding/action arrays
static void cleanup();                                // release resources
static void pollKeys();                               // poll hardware, set action flags
static void bind(int action, KeyCode key);            // add key→action binding
static void clearBindings(int action);                // remove all bindings for action
static bool action(int action);                       // true if action is active this frame
static const std::vector<KeyCode>& getBindings(int action);  // all keys bound to action
static std::string keyName(KeyCode key);              // human-readable key name (for HelpDisplay)
```

The game loop calls `pollKeys()` once per frame, then queries `action()` for each game action to build an `InputSnapshot`.

### Default Key Bindings

Set up in `TetrominosGame::bindDefaultKeys()` after `Input::init()`:

| Action | Keys |
|--------|------|
| Left | ArrowLeft, A, Numpad4 |
| Right | ArrowRight, D, Numpad6 |
| SoftDrop | ArrowDown, S, Numpad2 |
| HardDrop | Space, Numpad8 |
| RotateCW | ArrowUp, X, Numpad1, Numpad5, Numpad9 |
| RotateCCW | Z, Numpad3, Numpad7 |
| Hold | C, Numpad0 |
| Pause | Escape, F1 |
| Select | Enter |

### Linux Implementation (`InputLinux.cpp`)

**Polling model**: event-driven batch processing.

`pollKeys()` resets all action flags to `false`, then drains all pending input from stdin using `::select()` with **zero timeout** (non-blocking). It reads up to 64 bytes per iteration into a buffer and resolves each byte (or escape sequence) into `KeyCode` values:

- **ANSI escape sequences**: `ESC [ A/B/C/D` → arrow keys; `ESC [ number ~` → Insert/Delete/Home/End/PageUp/PageDown/F5–F12; `ESC O P/Q/R/S` → F1–F4
- **Control sequences**: bytes `0x01`–`0x1A` → emits `KeyCode::Control` + the corresponding letter
- **Single bytes**: letters (case-insensitive, stored uppercase), digits (emit both ASCII digit and Numpad equivalent since terminals can't distinguish), space, enter, tab, backspace, standalone escape

After collecting all pressed `KeyCode`s, it iterates over all actions and sets `s_actions[a] = true` if any bound key was pressed.

`init()` and `cleanup()` are no-ops (terminal setup is handled by `Platform`).

### Windows Implementation (`InputWin32.cpp`)

**Polling model**: direct state queries per action.

`pollKeys()` iterates over all actions. For each action, it checks every bound `KeyCode` by converting it to a Windows virtual key code via `toVK()` and calling `GetKeyState(vk) & 0x8000`. If any bound key is held, the action is active.

`toVK()` maps `KeyCode` values to Windows VK codes: uppercase letters and digits map directly, special keys use a switch table (`ArrowUp → VK_UP`, `F1 → VK_F1`, `Numpad0 → VK_NUMPAD0`, etc.).

### Comparison

| Aspect | Linux | Windows |
|--------|-------|---------|
| Polling | Batch drain stdin + cache flags | Iterate actions, query `GetKeyState()` |
| Key resolution | Parse raw bytes → KeyCode → action | KeyCode → VK → `GetKeyState()` |
| Arrow keys | Manual ANSI escape parsing | OS-provided virtual keys |
| Numpad | Emits both digit + Numpad KeyCode | Distinct VK codes |
| F-keys | ANSI sequences (`ESC O P`, `ESC [ 15~`, etc.) | Direct VK codes |

---

## 7. Platform Abstraction

**Files:** `KonsoleGE/source/Platform/Platform.h`, `KonsoleGE/source/Platform/PlatformLinux.cpp`, `KonsoleGE/source/Platform/PlatformWin32.cpp`

### Interface

```cpp
static void initConsole();
static void cleanupConsole();
static void flushInput();
static void flushOutput();
static int  getKey();              // blocking read (used by menus)
static int  getKeyTimeout(int ms); // blocking read with timeout (-1 if none)
static bool wasResized();
static bool isTerminalTooSmall();
static void showResizePrompt();
static int  offsetX();             // centering offsets for content
static int  offsetY();
static void updateOffsets();
static std::string getDataDir();   // persistent data directory
```

### Linux

**Console init** (`initConsole()`):
1. Saves original `termios` settings for restoration on exit
2. Enters alternate screen buffer (`\033[?1049h`) — restores the main screen on cleanup, like vim
3. Registers `SIGWINCH` handler for terminal resize detection (no `SA_RESTART` flag, so `read()` returns `EINTR` immediately — lets the menu loop detect resizes without waiting for input)
4. Sets raw mode via `tcsetattr()`:
   - `ICANON` off (no line buffering)
   - `ECHO` off
   - `ISIG` off (no Ctrl+C — quit via in-app menu)
   - `VMIN=1`, `VTIME=0` (blocking reads for menus; the game loop uses `select()` to avoid blocking)
   - `OPOST | ONLCR` on (so `\n` outputs as `\r\n`)
5. Hides cursor, sets console title, clears screen, resets colors, computes centering offsets

**Cleanup** (`cleanupConsole()`): restores original termios settings, shows cursor, resets colors, leaves alternate screen buffer (`\033[?1049l`). Guard flag `s_termiosRestored` prevents double-restore.

**`flushInput()`**: `tcflush(STDIN_FILENO, TCIFLUSH)` — discards unread terminal input.

**`getKey()`**: blocking read from stdin with ANSI escape sequence parsing. Reads one byte; if it's `ESC`, uses `select()` with 50ms timeout to read the rest of the escape sequence. Maps arrow keys to rlutil constants (`ESC[A` → UP, `ESC[B` → DOWN, etc.). This is separate from `Input::pollKeys()` — `getKey()` is for menus (blocking), `pollKeys()` is for the game loop (non-blocking).

**`getKeyTimeout(ms)`**: uses `select()` with the specified timeout on stdin. Returns `getKey()` if input arrives, or -1 on timeout.

**Resize handling**: `wasResized()` checks the `s_resized` flag (set by `SIGWINCH`). If the terminal is too small, shows a resize prompt and returns `false`. Otherwise, recomputes centering offsets (`updateOffsets()`), clears the screen, and returns `true` to trigger a full redraw.

**Centering**: computes offsets to center the 80×29 game area in larger terminals: `offsetX = max(0, (cols - 80) / 2)`, `offsetY = max(0, (rows - 29) / 2)`.

**Data directory**: `$XDG_DATA_HOME/Tetrominos` or `~/.local/share/Tetrominos`. Falls back to `/tmp/.local/share/Tetrominos` if `$HOME` is unset. Directory created with `mkdir()` at mode 0755.

### Windows

**Console init** (`initConsole()`):
1. `timeBeginPeriod(1)` — sets system timer resolution to 1ms so `Sleep()` is precise (default ~15.6ms granularity halves the frame rate)
2. Sets console output to UTF-8 via `SetConsoleOutputCP(CP_UTF8)` (required for box-drawing characters ╔═║ and block characters ██░░)
3. Disables Ctrl+C via `SetConsoleCtrlHandler(NULL, TRUE)` — quit via in-app menu only
4. Disables resize/maximize buttons via `SetWindowLongPtr` (cosmetic — Windows Terminal ignores this)
5. Sets window title to "Tetrominos"
6. Resets colors (`system("color 0F")`), hides cursor
7. Enables `ENABLE_VIRTUAL_TERMINAL_PROCESSING` on the output handle so ANSI escape sequences are interpreted by Windows Terminal
8. Sets 80×29 screen buffer via console APIs (legacy conhost), then sends `\033[8;29;80t` xterm resize sequence (Windows Terminal)
9. Disables quick-edit mode (prevents accidental selection-pause)
10. Installs `BatchingStreambuf` on `std::cout` (see Output Batching below)

**Output Batching** (`BatchingStreambuf`): A custom `std::streambuf` subclass defined in `PlatformWin32.cpp` that replaces `std::cout`'s default streambuf. All `std::cout <<` operations append to an in-memory string (zero system calls). When `std::flush` triggers `sync()`, the entire buffer is written to the console in a single `WriteFile` call. This avoids hundreds of individual round-trips through Windows Terminal's ConPTY layer per frame. Without this, rendering is ~10× slower on Windows Terminal compared to Linux terminals. Installed after all init output is flushed; uninstalled during cleanup.

**Cleanup** (`cleanupConsole()`): uninstalls the batching streambuf (flushing any pending output and restoring the original `std::cout` streambuf), then calls `timeEndPeriod(1)` to restore the default timer resolution.

**`flushInput()`**: `FlushConsoleInputBuffer()` — discards unread console input events.

**`flushOutput()`**: `std::cout << std::flush` — triggers `BatchingStreambuf::sync()`, which writes the accumulated frame output to the console via a single `WriteFile` call.

**`getKey()`**: polls `_kbhit()` in a loop with `Sleep(10)` per iteration, returning `rlutil::getkey()` when a key is available. Also checks for terminal resize each iteration — if the terminal is no longer 80×29, returns -1 immediately so the caller's `wasResized()` check runs. This mirrors how Linux's `SIGWINCH` interrupts `read()` to allow resize detection between keypresses.

**`getKeyTimeout(ms)`**: polls `_kbhit()` in a loop with `Sleep(1)` per iteration, returns `getKey()` if a key is available or -1 on timeout.

**Resize handling**: `wasResized()` checks if `rlutil::tcols()` or `rlutil::trows()` differ from 80×29. If so, sends `\033[8;29;80t` to snap back, waits 50ms for the terminal to process it, clears the screen, and returns `true` for a full redraw. `isTerminalTooSmall()` performs the same size check. Offsets always return 0 (content is not centered within the window).

**Data directory**: `%APPDATA%\Tetrominos` via `SHGetFolderPathA()`. Falls back to `.\Tetrominos` if the shell API call fails.

---

## 8. Panel Rendering System

**Files:** `KonsoleGE/source/UI/Panel.h/.cpp`, `KonsoleGE/source/UI/RowDrawContext.h/.cpp`, `KonsoleGE/source/UI/Icon.h/.cpp`, `KonsoleGE/source/UI/Color.h`, `Game/Display/PiecePreview.h/.cpp`, `Game/Display/PieceDisplay.h/.cpp`, `Game/Display/PlayfieldDisplay.h/.cpp`, `Game/Display/ScoreDisplay.h/.cpp`, `Game/Display/HighScoreDisplay.h/.cpp`, `Game/Display/HelpDisplay.h/.cpp`, `Game/Display/Confetti.h/.cpp`

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

Composite display for the "Next" and "Hold" panels. Constructor takes a size (number of preview slots). The Next panel is configurable (0-6 slots, default `NEXT_PIECE_QUEUE_SIZE` = 6); the Hold panel always has 1 slot.

- Size 1 (Hold): title + separator + 1 preview slot
- Size N (Next): title + separator + 1 main slot + separator + (N-1) queue slots

`update(vector<const Tetrimino*>)` sets each slot directly from the vector — slot 0 gets the first piece, slot 1 the second, etc. `rebuild(size)` recreates the panel with a different number of slots (used when `GameRenderer::configure()` changes the preview count). `clear()` erases the panel from the screen.

### PlayfieldDisplay

The 10x20 visible game field. Its `PlayfieldElement` (PanelElement subclass, height = 20) renders each row by checking, in priority order:
1. Current tetrimino mino → `██` in piece color
2. Locked mino → `░░` with locked color as background
3. Ghost piece → `██` in dark grey
4. Hard drop trail → `░░` in piece color (foreground only, fades top-to-bottom)
5. Empty cell → checkerboard pattern (`░░` / `▒▒` alternating in dark grey)

### ScoreDisplay

Panel (interior width 18) showing Score, Time, TPM, LPM, Level, Goal (optional), Lines, Quad, Combos, and T-Spins. Score color changes to green during back-to-back bonus. Values are zero-padded to fixed widths. `configure(showGoal)` controls whether the Goal row appears (shown in Marathon, hidden in Sprint/Ultra). Has two update paths:

- `update(state)` — full update of all fields (called when dirty)
- `updateTimer(state)` — updates only Time, TPM, and LPM (called every frame for smooth display)

### HighScoreDisplay

Standalone two-panel viewer (not a game HUD element). Has two entry points:

- `open(allHighscores, initialVariant)` — browse mode, opened from the main menu
- `openForNewEntry(allHighscores, newRecord, variant)` — new high score mode, returns the player name. Shows confetti animation.

Takes `const HighScoreTable&` (per-variant scores) and a `GameVariant`. Blocks until the user presses ENTER or ESC.

**Left panel** (interior width 28): title "HIGH SCORES", separator, 10 ranked entries. Each entry shows cursor indicator, rank, name (10 chars), and score (10 digits). UP/DOWN arrows move the selection cursor.

**Right panel** (interior width 22): variant tabs at top (Marathon / Sprint / Ultra, switchable with LEFT/RIGHT). Three header rows (score, time, name), a separator, seven stat rows (Level, TPM, LPM, Lines, Quad, Combos, T-Spins), a separator, and five option rows (Start, Mode, Ghost, Hold, Preview) showing the settings used during that game. Empty slots show dashes.

Both panels are centered side-by-side with a 1-char gap, positioned relative to the window center.

### HelpDisplay

Standalone two-panel key bindings reference, accessible from the main menu. Reads the current bindings from `Input::getBindings()` for each action (except Select).

**Layout**: two panels side-by-side. Each panel lists actions with their bound keys (up to `kMaxKeyCols = 3` keys per action). `refreshBindings()` updates the displayed key names via `Input::keyName()`. Blocks until the user presses ENTER or ESC.

### Confetti

Particle animation system for celebrating new high scores. `start(screenW, screenH, offsetX, offsetY, exclusionZones)` spawns colored particles that fall across the screen. `update()` advances particle positions each frame. Particles avoid exclusion zones (the panels) to prevent overdrawing UI elements. `stop()` clears all particles and terminates the animation.

### Icon

Standalone single-character display (not a PanelElement). Available as a general-purpose UI indicator in the KonsoleGE layer.

---

## 9. Sound Engine

**Files:** `KonsoleGE/source/Util/SoundEngine.h/.cpp`

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

`playMusic(name)` stops the current track, seeks to frame 0, sets volume, and starts. `pauseMusic()` / `unpauseMusic()` pause and resume the current track (used during the pause menu). The `Tetrominos` facade checks `musicEnded()` each frame and advances to the next track based on the active `SoundtrackMode`.

### Volume Control

Music and effect volumes are independently adjustable via `setMusicVolume(float)` / `getMusicVolume()` and `setEffectVolume(float)` / `getEffectVolume()`. Volume values range from 0.0 (silent) to 1.0 (full). These are exposed in the Options and Pause Sound menus as 11-step visual sliders (0–10 hash marks).

### Soundtrack Mode

`SoundtrackMode` controls how game music tracks advance when a track ends:

| Mode | Behavior |
|------|----------|
| `Cycle` | Play A → B → C → A (default) |
| `Random` | Play a random different track |
| `TrackA` | Always play track A |
| `TrackB` | Always play track B |
| `TrackC` | Always play track C |

Set via `setSoundtrackMode()` / `getSoundtrackMode()`. Exposed in the Options and Pause Sound menus. When the soundtrack mode is changed during a pause, the `Tetrominos` facade detects the mismatch on resume and switches to the correct track.

---

## 10. Timer

**Files:** `KonsoleGE/source/Util/Timer.h/.cpp`

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
- `"harddroptrail"` — 150ms hard drop trail animation

Elapsed time is computed on-the-fly from `steady_clock::now() - start_time` — no cached values, no drift.

---

## 11. Random

**Files:** `KonsoleGE/source/Util/Random.h/.cpp`

### Design

Static utility class (constructors deleted). Uses a `std::mt19937` Mersenne Twister generator as a Meyers singleton (static local in `getGenerator()`), seeded once from `std::random_device`.

### API

```cpp
static int getInteger(int min, int max);  // uniform random in [min, max] inclusive
```

Creates a fresh `uniform_int_distribution` for each call. Used by the bag shuffle (Fisher-Yates) to generate random swap indices.

---

## 12. Menu System

**Files:** `KonsoleGE/source/UI/Menu.h/.cpp`

### Structure

A hierarchical, tree-based navigation system. Each `Menu` owns:

- `_options`: ordered list of item names
- `_menus`: submenu mappings (name -> `Menu*`)
- `_callbacks`: action callbacks (name -> `function<void(OptionChoice)>`)
- `_actionCallbacks`: void callbacks for options that trigger immediate actions (e.g. opening a submenu with pre-configuration)
- `_optionsValues` / `_optionsValuesChoices`: for items with selectable values (cycled with LEFT/RIGHT)
- `_closeOptions` / `_closeAllMenusOptions`: items that close the current menu or the entire menu tree
- `_optionHints` / `_optionValueHints`: hint text shown below the menu for the selected option or its current value

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

Delegates to a `Panel`. Selected item is prefixed with `"> "`, others with `"  "`. Items with values show `"Option : Value"`. An optional hint panel below the menu shows context-sensitive help text for the selected option or its current value (set via `setOptionHint()` and `setOptionValueHint()`). Layout is auto-centered in the 80x29 area and regenerated on terminal resize.

### Integration

Two static function pointers allow external control:
- `Menu::shouldExitGame` — checked each loop iteration to break out when the game exits
- `Menu::onResize` — called on terminal resize to redraw surrounding UI (also used by `HighScoreDisplay`)

Menu callbacks are wired by `GameMenus::configure()` using lambdas that capture the `Tetrominos` reference. The menu tree:

```
Main Menu
├── New Game → Level (1-15), Variant (Marathon/Sprint/Ultra) + Start
├── Options → Lock Down, Ghost Piece, Hold Piece, Preview (0-6),
│             Music, Effects, Soundtrack, Reset Defaults, Back
├── High Scores → HighScoreDisplay (per-variant two-panel viewer)
├── Help → HelpDisplay (two-panel key bindings reference)
├── Test → TestRunner (debug builds only, GAME_DEBUG)
└── Exit → Quit confirmation
Pause Menu
├── Resume
├── Restart → Confirmation
├── Options → Sound submenu (Music, Effects, Soundtrack, Back)
├── Main Menu → Confirmation
└── Exit Game → Quit confirmation
Game Over Menu
├── Retry
├── Main Menu
└── Exit Game → Quit confirmation
```

---

## 13. Build System & Media Embedding

**Files:** `CMakeLists.txt`, `KonsoleGE/scripts/embed_media.py`

### CMake Configuration

- C++17 standard, required
- `CMAKE_EXPORT_COMPILE_COMMANDS ON` for clangd
- Two build targets: `konsolege` (static library) and `tetrominos`/`Tetrominos` (executable)
- Binary name: `tetrominos` (Linux/macOS), `Tetrominos.exe` (Windows)
- Sources gathered with `file(GLOB_RECURSE ...)` to pick up files in subfolders automatically

### Build Targets

**`konsolege`** — static library from `KonsoleGE/source/**/*.cpp` (git submodule). Included via `add_subdirectory(KonsoleGE)`. Its include directories and link dependencies are `PUBLIC`, so they propagate automatically to the executable via `target_link_libraries`.

**`tetrominos`** — executable from `Game/**/*.cpp` + `media_data.cpp`. Links against `konsole` with `PRIVATE` visibility.

A `media_embed` custom target ensures `media_data.h` is generated before `konsole` compiles (SoundEngine needs it).

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

Shared via `add_compile_options()` (applies to both targets):

- **GCC/Clang**: `-Wall -Wextra -Wpedantic -Wshadow -Wconversion`
- **MSVC**: `/W3 /sdl`

Vendored headers in `Tetrominos/include/` use `-isystem` to suppress their warnings.

### Platform Linking

Attached to `konsole` with `PUBLIC` visibility (propagates to the executable):

| Platform | Libraries |
|----------|-----------|
| Windows | `winmm`, `ole32` |
| macOS | `pthread`, `dl`, `m`, CoreAudio, AudioToolbox, CoreFoundation |
| Linux | `pthread`, `dl`, `m` |

### Media Embedding Pipeline

`KonsoleGE/scripts/embed_media.py` runs at build time via `add_custom_command`, triggered when any file in `Tetrominos/media/` changes:

1. Scans `Tetrominos/media/` for all files
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
| `quad.wav` | Effect | 4-line clear (Quad) |
