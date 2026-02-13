#pragma once

#include <deque>
#include <array>

#define TETRIS_HEIGHT 40
#define TETRIS_WIDTH 10
#define BUFFER_START 0
#define BUFFER_END 19
#define MATRIX_START 20
#define MATRIX_END 39
#define SKYLINE_START 18

using MatrixRow = std::array<int, TETRIS_WIDTH>;
using GameMatrix = std::deque<MatrixRow>;
