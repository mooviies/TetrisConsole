#pragma once

#include <deque>
#include <array>

constexpr int TETRIS_HEIGHT = 40;
constexpr int TETRIS_WIDTH = 10;
constexpr int BUFFER_START = 0;
constexpr int BUFFER_END = 19;
constexpr int MATRIX_START = 20;
constexpr int MATRIX_END = 39;
constexpr int SKYLINE_START = 18;
constexpr int MIN_LEVEL = 1;
constexpr int MAX_LEVEL = 15;

enum class  VARIANT { MARATHON, SPRINT, ULTRA };
inline constexpr size_t VARIANT_COUNT = 3;
enum class LOCKDOWN_MODE { EXTENDED, EXTENDED_INFINITY, CLASSIC };
enum class DROP_TYPE { NORMAL, SOFT, HARD };

using MatrixRow = std::array<int, TETRIS_WIDTH>;
using GameMatrix = std::deque<MatrixRow>;
