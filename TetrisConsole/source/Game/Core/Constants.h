#pragma once

#include <deque>
#include <array>

constexpr int BOARD_HEIGHT = 40;
constexpr int BOARD_WIDTH = 10;
constexpr int BUFFER_START = 0;
constexpr int BUFFER_END = 19;
constexpr int MATRIX_START = 20;
constexpr int MATRIX_END = 39;
constexpr int SKYLINE_START = 18;
constexpr int MIN_LEVEL = 1;
constexpr int MAX_LEVEL = 15;
constexpr int VISIBLE_ROWS = MATRIX_END - MATRIX_START + 1;
constexpr int OVERLAY_LEVEL_THRESHOLD = 10;

enum class GameVariant { Marathon, Sprint, Ultra };
inline constexpr size_t VARIANT_COUNT = 3;
enum class LockDownMode { Extended, ExtendedInfinity, Classic };
enum class DropType { Normal, Soft, Hard };

using MatrixRow = std::array<int, BOARD_WIDTH>;
using GameMatrix = std::deque<MatrixRow>;
