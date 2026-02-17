#include "Tetris.h"

#include <iostream>

#include "HighScoreDisplay.h"
#include "Timer.h"
#include "Menu.h"
#include "SoundEngine.h"
#include "rlutil.h"

Tetris::Tetris(Menu &pauseMenu, Menu &gameOverMenu, HighScoreDisplay &highScoreDisplay)
    : _controller(Timer::instance()), _pauseMenu(pauseMenu), _gameOverMenu(gameOverMenu),
      _highScoreDisplay(highScoreDisplay)
{
    _state.loadOptions();
    _state.loadHighscore();
}

Tetris::~Tetris() = default;

void Tetris::start() {
    _controller.configurePolicies(_state.config.mode);
    _controller.configureVariant(_state.config.variant, _state);
    _renderer.configure(_state.config.previewCount, _state.config.holdEnabled, _state.config.showGoal);
    _controller.start(_state);
    _renderer.invalidate();
    _renderer.render(_state);
    SoundEngine::playMusic("A");
}

void Tetris::step(const InputSnapshot& input) {
    const StepResult result = _controller.step(_state, input);

    playPendingSounds();

    if (input.mute && !_wasMutePressed)
        SoundEngine::cycleMute();
    _wasMutePressed = input.mute;

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
            if (!_wasPausePressed)
                handlePause();
            break;
        case StepResult::GameOver:
            handleGameOver();
            break;
    }
    _wasPausePressed = input.pause;
}

void Tetris::render() {
    if (_state.isDirty()) {
        _renderer.render(_state);
        _state.clearDirty();
    } else {
        _renderer.renderTimer(_state);
    }
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

    const OptionChoice choices = _pauseMenu.open(false, true);
    const auto& selected = choices.options[choices.selected];

    if (selected == "Restart") {
        SoundEngine::stopMusic();
        _controller.start(_state);
        _renderer.configure(_state.config.previewCount, _state.config.holdEnabled, _state.config.showGoal);
        _renderer.invalidate();
        _renderer.render(_state);
        _state.clearDirty();
        SoundEngine::playMusic("A");
        return;
    }

    if (selected == "Main Menu") {
        SoundEngine::stopMusic();
        _backToMenu = true;
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
    if (_state.stats.hasBetterHighscore) {
        HighScoreRecord rec{};
        rec.score         = _state.stats.score;
        rec.level         = _state.stats.level;
        rec.lines         = _state.stats.lines;
        rec.tpm           = _state.tpm();
        rec.lpm           = _state.lpm();
        rec.tetris        = _state.stats.tetris;
        rec.combos        = _state.stats.combos;
        rec.tSpins        = _state.stats.tSpins;
        rec.gameElapsed   = _state.gameElapsed();
        rec.startingLevel = _state.config.startingLevel;
        rec.mode          = _state.config.mode;
        rec.ghostEnabled  = _state.config.ghostEnabled;
        rec.holdEnabled   = _state.config.holdEnabled;
        rec.previewCount  = _state.config.previewCount;
        rlutil::cls();
        GameRenderer::renderTitle("A classic in console!");
        _state.setPlayerName(_highScoreDisplay.openForNewEntry(_state.allHighscores(), rec, _state.config.variant));
        rlutil::cls();
        GameRenderer::renderTitle("A classic in console!");
    }
    _state.saveHighscore();

    const OptionChoice choices = _gameOverMenu.open(_state.stats.hasBetterHighscore);
    const auto& selected = choices.options[choices.selected];

    if (selected == "Main Menu") {
        _backToMenu = true;
        return;
    }

    _controller.start(_state);
    _renderer.configure(_state.config.previewCount, _state.config.holdEnabled, _state.config.showGoal);
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

