# Tetris Console

A cross-platform Tetris game that runs in the terminal, written in C++17. Features modern Tetris Guideline mechanics including SRS wall kicks, T-spins, hold piece, back-to-back bonuses, and three game modes.

All media (music and sound effects) are embedded into the binary at compile time — no external files needed to play.

## Building

Requires CMake 3.16+ and a C++17 compiler. Python 3 is needed for media embedding.

**Linux / macOS:**
```
cmake -B cmake-build-debug
cmake --build cmake-build-debug
./cmake-build-debug/tetris
```

**Windows (Visual Studio):**
```
cmake -B build
cmake --build build
build\TetrisConsole.exe
```

The terminal should be at least 80 columns wide and 29 rows tall.

## Controls

| Action     | Arrows/Keys | Letters | Numpad      |
|------------|-------------|---------|-------------|
| Move left  | Left        | A       | 4           |
| Move right | Right       | D       | 6           |
| Soft drop  | Down        | S       | 2           |
| Hard drop  | Space       |         | 8           |
| Rotate CW  | Up          | X       | 1, 5, 9     |
| Rotate CCW |             | Z       | 3, 7        |
| Hold piece |             | C       | 0           |
| Pause      | Escape, F1  |         |             |
| Cycle mute |             | M       |             |
| Select     | Enter       |         |             |

## Options

All options are persisted across sessions.

**Starting Level** (1–15) — Higher levels have faster gravity.

**Lock Down** — Controls what happens when a piece lands:
- **Extended** (default) — 0.5s lock delay, resets on move/rotate, up to 15 resets
- **Infinite** — 0.5s lock delay, unlimited resets
- **Classic** — Locks immediately when the piece can't fall

**Ghost Piece** (On/Off) — Shows a preview of where the current piece will land.

**Hold Piece** (On/Off) — Allows swapping the current piece once per drop.

**Preview** (0–6) — Number of next pieces shown in the queue.

**Variant** — Game mode:
- **Marathon** (default) — Classic Tetris. Level up every time you clear the goal, scoring as much as possible.
- **Sprint** — Clear 40 lines as fast as you can.
- **Ultra** — Score as many points as possible in 2 minutes.

## Game Modes

**Marathon** — The classic Tetris experience. Clear lines to advance through levels, with gravity increasing at each level. The goal increases per level (level × 5 lines). Your score is the primary measure of success.

**Sprint** — A race against the clock. Clear 40 lines as quickly as possible. The level stays fixed at your starting level (no level-ups). The game ends as soon as 40 lines are cleared.

**Ultra** — A timed challenge. Score as many points as possible within a 2-minute time limit. The level stays fixed at your starting level. A countdown timer replaces the elapsed time display.

## Features

- Three game modes: Marathon, Sprint (40-line race), and Ultra (2-minute time attack)
- 10x20 playfield with standard 7-bag piece randomization
- SRS (Super Rotation System) wall kicks with 5 test points per rotation
- Hold piece and configurable next piece preview (0-6 pieces)
- T-spin and mini T-spin detection with bonus scoring
- Back-to-back bonus (50% extra) for consecutive Tetrises or T-spins
- Combo (Ren) tracking with text notifications for impressive moves
- 15 levels with Guideline gravity speeds
- In-game stats: score, time, TPM, LPM, level, lines, goal, Tetrises, combos, T-spins
- Per-variant high score leaderboards (top 10 each) with detailed stats and game options per entry
- Confetti animation on new high scores
- Help screen showing all key bindings (accessible from the main menu)
- Options menu: lock-down mode, ghost piece, hold piece, preview count — persisted across sessions
- Mute toggle (M key): cycles unmuted (white ♪) → music muted (yellow ♪) → all muted (red ♪)
- Streamed music (three tracks that cycle A/B/C) and sound effects via miniaudio
- UTF-8 box-drawing and block characters for the UI
- Persistent data in `$XDG_DATA_HOME/TetrisConsole/` (Linux) or `%APPDATA%\TetrisConsole\` (Windows)

## Scoring

All base points are multiplied by the current level. Consecutive Tetrises or T-spin line clears earn a 1.5x back-to-back bonus.

| Action           | Points |
|------------------|--------|
| Single           | 100    |
| Double           | 300    |
| Triple           | 500    |
| Tetris           | 800    |
| T-Spin Single    | 400    |
| T-Spin Double    | 800    |
| T-Spin Triple    | 1200   |
| Mini T-Spin      | 100    |
| Soft drop        | 1/line |
| Hard drop        | 2/line |

## Project Structure

```
TetrisConsole/source/
  Konsole/                  # Static library — platform abstraction & UI primitives
    Platform/               # OS abstraction (console init, keyboard input)
    UI/                     # Panel rendering, menus, colors
    Util/                   # Timer, RNG, audio engine, helpers
  Tetris/                   # Executable — game logic (links against Konsole)
    Core/                   # MVC triad, facade, entry point
    Piece/                  # Tetrimino geometry, SRS rotation data
    Rules/                  # Pluggable gameplay policies (scoring, gravity, lock-down, goals, variants)
    Display/                # HUD and modal display components
    Test/                   # Debug-only test runner
```

Konsole is built as a static library that the Tetris executable links against, enforcing a clean dependency boundary.

## Platform Support

- **Linux** — termios raw mode, ANSI escape sequences, POSIX audio via miniaudio
- **Windows** — Win32 Console API, native audio via miniaudio
- **macOS** — Same as Linux, with CoreAudio backend

## Dependencies

All dependencies are vendored as header-only libraries in `include/`:

- [miniaudio](https://miniaud.io/) — cross-platform audio
- [rlutil](https://github.com/tapio/rlutil) — console colors and cursor positioning

## License

See [LICENSE](LICENSE) for details.
