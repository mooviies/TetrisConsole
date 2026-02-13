#include "Tetris.h"

#include "Timer.h"

Tetris::Tetris(Menu &pauseMenu, Menu &gameOverMenu)
    : _controller(Timer::instance(), pauseMenu, gameOverMenu, _renderer) {}

Tetris::~Tetris() = default;
