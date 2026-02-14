#include "Tetris.h"

#include "Timer.h"
#include "Menu.h"
#include "Input.h"
#include "SoundEngine.h"
#include "Utility.h"

Tetris::Tetris(Menu &pauseMenu, Menu &gameOverMenu)
    : _controller(Timer::instance()), _pauseMenu(pauseMenu), _gameOverMenu(gameOverMenu) {}

Tetris::~Tetris() = default;

void Tetris::start() {
    _controller.start(_state);
    _renderer.invalidate();
    _renderer.render(_state);
    SoundEngine::playMusic("A");
}

void Tetris::step() {
    const StepResult result = _controller.step(_state);

    // Mute toggle (UI concern, not game logic)
    {
        static bool wasMutePressed = false;
        const bool mutePressed = Input::mute();
        if (mutePressed && !wasMutePressed)
            SoundEngine::cycleMute();
        wasMutePressed = mutePressed;
    }

    switch (result) {
        case StepResult::Continue:
            break;
        case StepResult::PauseRequested:
            handlePause();
            break;
        case StepResult::GameOver:
            handleGameOver();
            break;
    }
}

void Tetris::render() {
    if (!_state.isDirty())
        return;
    _state.updateHighscore();
    _renderer.render(_state);
    _state.clearDirty();
}

void Tetris::redraw() {
    Utility::showTitle("A classic in console!");
    _renderer.invalidate();
    _state.updateHighscore();
    _renderer.render(_state);
    _state.clearDirty();
}

void Tetris::handlePause() {
    SoundEngine::pauseMusic();
    _state.updateHighscore();
    _renderer.render(_state, false);
    _state.clearDirty();

    if (const OptionChoice choices = _pauseMenu.open(false, true);
        choices.options[choices.selected] == "Restart") {
        SoundEngine::stopMusic();
        _controller.reset(_state);
        _renderer.invalidate();
        _renderer.render(_state);
        _state.clearDirty();
        SoundEngine::playMusic("A");
        return;
    }

    _renderer.invalidate();
    _state.updateHighscore();
    _renderer.render(_state);
    _state.clearDirty();
    SoundEngine::unpauseMusic();
}

void Tetris::handleGameOver() {
    SoundEngine::stopMusic();
    _state.updateHighscore();
    _state.saveHighscore();

    _gameOverMenu.open(_state.hasBetterHighscore());

    _controller.reset(_state);
    _renderer.invalidate();
    _renderer.render(_state);
    _state.clearDirty();
    SoundEngine::playMusic("A");
}
