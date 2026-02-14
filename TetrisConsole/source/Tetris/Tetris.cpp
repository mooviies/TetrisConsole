#include "Tetris.h"

#include "Timer.h"
#include "Menu.h"
#include "SoundEngine.h"

Tetris::Tetris(Menu &pauseMenu, Menu &gameOverMenu)
    : _controller(Timer::instance()), _pauseMenu(pauseMenu), _gameOverMenu(gameOverMenu)
{
    _state.loadHighscore();
}

Tetris::~Tetris() = default;

void Tetris::start() {
    _controller.start(_state);
    _renderer.invalidate();
    _renderer.render(_state);
    SoundEngine::playMusic("A");
}

void Tetris::step() {
    const StepResult result = _controller.step(_state);

    playPendingSounds();

    if (_state.muteRequested()) {
        SoundEngine::cycleMute();
        _state.clearMuteRequested();
    }

    if (SoundEngine::musicEnded()) {
        const auto& name = SoundEngine::currentMusicName();
        if (name == "A") SoundEngine::playMusic("B");
        else if (name == "B") SoundEngine::playMusic("C");
        else if (name == "C") SoundEngine::playMusic("A");
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
    _renderer.render(_state);
    _state.clearDirty();
}

void Tetris::redraw() {
    GameRenderer::renderTitle("A classic in console!");
    _renderer.invalidate();
    _renderer.render(_state);
    _state.clearDirty();
}

void Tetris::handlePause() {
    _state.pauseGameTimer();
    SoundEngine::pauseMusic();
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
    _renderer.render(_state);
    _state.clearDirty();
    SoundEngine::unpauseMusic();
    _state.resumeGameTimer();
}

void Tetris::handleGameOver() {
    _state.pauseGameTimer();
    SoundEngine::stopMusic();
    _state.saveHighscore();

    _gameOverMenu.open(_state.hasBetterHighscore());

    _controller.reset(_state);
    _renderer.invalidate();
    _renderer.render(_state);
    _state.clearDirty();
    SoundEngine::playMusic("A");
}

void Tetris::playPendingSounds() {
    for (const auto sound : _state.pendingSounds()) {
        switch (sound) {
            case GameSound::Click:     SoundEngine::playSound("CLICK"); break;
            case GameSound::Lock:      SoundEngine::playSound("LOCK"); break;
            case GameSound::HardDrop:  SoundEngine::playSound("HARD_DROP"); break;
            case GameSound::LineClear: SoundEngine::playSound("LINE_CLEAR"); break;
            case GameSound::Tetris:    SoundEngine::playSound("TETRIS"); break;
        }
    }
    _state.clearPendingSounds();
}
