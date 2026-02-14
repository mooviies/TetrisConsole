# Tetris Console

A cross-platform Tetris game that runs in the terminal, written in C++17. Features modern Tetris Guideline mechanics including SRS wall kicks, T-spins, hold piece, back-to-back bonuses, and three game modes.

All media (music and sound effects) are embedded into the binary at compile time — no external files needed to play.

## Building

Requires CMake 3.16+ and a C++17 compiler. Python 3 is needed for media embedding.

```
cmake -B build
cmake --build build
./build/tetris
```

The terminal should be at least 80 columns wide and 29 rows tall.

## Controls

| Key              | Action                  |
|------------------|-------------------------|
| Left / Right     | Move piece              |
| Down             | Soft drop               |
| Space            | Hard drop               |
| Up / X           | Rotate clockwise        |
| Z                | Rotate counter-clockwise|
| C                | Hold piece              |
| M                | Cycle mute              |
| Escape           | Pause                   |
| Enter            | Select (menus)          |

## Game Modes

- **Extended** — Modern Tetris with lock delay resets (up to 15 moves before forced lock)
- **Infinite** — Same as Extended, but with unlimited lock delay resets
- **Classic** — Traditional rules without lock delay resets

## Features

- 10x20 playfield with standard 7-bag piece randomization
- SRS (Super Rotation System) wall kicks with 5 test points per rotation
- Hold piece and configurable next piece preview (0-6 pieces)
- T-spin and mini T-spin detection with bonus scoring
- Back-to-back bonus (50% extra) for consecutive Tetrises or T-spins
- Combo (Ren) tracking
- 15 levels with Guideline gravity speeds
- In-game stats: score, time, TPM, LPM, level, lines, Tetrises, combos, T-spins
- Top-10 high score leaderboard with detailed stats and game options per entry
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
