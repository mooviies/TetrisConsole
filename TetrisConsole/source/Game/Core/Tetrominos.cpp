#include "Tetrominos.h"

#include <iostream>

#include "HighScoreDisplay.h"
#include "Random.h"
#include "Timer.h"
#include "Menu.h"
#include "SoundEngine.h"
#include "rlutil.h"

Tetrominos::Tetrominos(Menu &pauseMenu, Menu &gameOverMenu, HighScoreDisplay &highScoreDisplay)
    : _controller(Timer::instance()), _pauseMenu(pauseMenu), _gameOverMenu(gameOverMenu),
      _highScoreDisplay(highScoreDisplay) {
    _state.loadOptions();
    _state.loadHighscore();
}

Tetrominos::~Tetrominos() = default;

void Tetrominos::start() {
    _controller.configurePolicies(_state.config.mode);
    _controller.configureVariant(_state.config.variant, _state);
    _renderer.configure(_state.config.previewCount, _state.config.holdEnabled, _state.config.showGoal);
    _controller.start(_state);
    _renderer.invalidate();
    _renderer.render(_state);
    playStartingMusic();
}

void Tetrominos::step(const InputSnapshot &input) {
    const StepResult result = _controller.step(_state, input);

    playPendingSounds();

    if (SoundEngine::musicEnded()) {
        const auto &name = SoundEngine::currentMusicName();
        switch (SoundEngine::getSoundtrackMode()) {
            case SoundtrackMode::Cycle:
                if (name == "A") SoundEngine::playMusic("B");
                else if (name == "B") SoundEngine::playMusic("C");
                else if (name == "C") SoundEngine::playMusic("A");
                break;
            case SoundtrackMode::Random:
                SoundEngine::playMusic(randomTrack(name));
                break;
            case SoundtrackMode::TrackA: SoundEngine::playMusic("A"); break;
            case SoundtrackMode::TrackB: SoundEngine::playMusic("B"); break;
            case SoundtrackMode::TrackC: SoundEngine::playMusic("C"); break;
        }
    }

    switch (result) {
        case StepResult::Continue: break;
        case StepResult::PauseRequested:
            if (!_wasPausePressed) handlePause();
            break;
        case StepResult::GameOver: handleGameOver(); break;
    }
    _wasPausePressed = input.pause;
}

void Tetrominos::render() {
    if (_state.isDirty()) {
        _renderer.render(_state);
        _state.clearDirty();
    } else {
        _renderer.renderTimer(_state);
    }
}

void Tetrominos::redraw() {
    GameRenderer::renderTitle("A classic in console!");
    _renderer.invalidate();
    _renderer.render(_state);
    _state.clearDirty();
}

void Tetrominos::handlePause() {
    _state.pauseGameTimer();
    SoundEngine::pauseMusic();
    _renderer.render(_state, false);
    _state.clearDirty();

    const OptionChoice choices = _pauseMenu.open(false, true);
    const auto &selected = choices.options[choices.selected];

    if (selected == "Restart") {
        SoundEngine::stopMusic();
        _controller.start(_state);
        _renderer.configure(_state.config.previewCount, _state.config.holdEnabled, _state.config.showGoal);
        _renderer.invalidate();
        _renderer.render(_state);
        _state.clearDirty();
        playStartingMusic();
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

    const auto &current = SoundEngine::currentMusicName();
    switch (SoundEngine::getSoundtrackMode()) {
        case SoundtrackMode::TrackA:
            if (current != "A") SoundEngine::playMusic("A"); else SoundEngine::unpauseMusic();
            break;
        case SoundtrackMode::TrackB:
            if (current != "B") SoundEngine::playMusic("B"); else SoundEngine::unpauseMusic();
            break;
        case SoundtrackMode::TrackC:
            if (current != "C") SoundEngine::playMusic("C"); else SoundEngine::unpauseMusic();
            break;
        default: SoundEngine::unpauseMusic(); break;
    }

    _state.resumeGameTimer();
}

void Tetrominos::handleGameOver() {
    _state.pauseGameTimer();
    SoundEngine::stopMusic();
    if (_state.stats.hasBetterHighscore) {
        HighScoreRecord rec{};
        rec.score = _state.stats.score;
        rec.level = _state.stats.level;
        rec.lines = _state.stats.lines;
        rec.tpm = _state.tpm();
        rec.lpm = _state.lpm();
        rec.quad = _state.stats.quad;
        rec.combos = _state.stats.combos;
        rec.tSpins = _state.stats.tSpins;
        rec.gameElapsed = _state.gameElapsed();
        rec.startingLevel = _state.config.startingLevel;
        rec.mode = _state.config.mode;
        rec.ghostEnabled = _state.config.ghostEnabled;
        rec.holdEnabled = _state.config.holdEnabled;
        rec.previewCount = _state.config.previewCount;
        rlutil::cls();
        GameRenderer::renderTitle("A classic in console!");
        _state.setPlayerName(_highScoreDisplay.openForNewEntry(_state.allHighscores(), rec, _state.config.variant));
        rlutil::cls();
        GameRenderer::renderTitle("A classic in console!");
    }
    _state.saveHighscore();

    const OptionChoice choices = _gameOverMenu.open(_state.stats.hasBetterHighscore);
    const auto &selected = choices.options[choices.selected];

    if (selected == "Main Menu") {
        _backToMenu = true;
        return;
    }

    _controller.start(_state);
    _renderer.configure(_state.config.previewCount, _state.config.holdEnabled, _state.config.showGoal);
    _renderer.invalidate();
    _renderer.render(_state);
    _state.clearDirty();
    playStartingMusic();
}

void Tetrominos::playPendingSounds() {
    for (const auto sound : _state.pendingSounds()) {
        switch (sound) {
            case GameSound::Click: SoundEngine::playSound("CLICK"); break;
            case GameSound::Lock: SoundEngine::playSound("LOCK"); break;
            case GameSound::HardDrop: SoundEngine::playSound("HARD_DROP"); break;
            case GameSound::LineClear: SoundEngine::playSound("LINE_CLEAR"); break;
            case GameSound::Quad: SoundEngine::playSound("QUAD"); break;
        }
    }
    _state.clearPendingSounds();
}

std::string Tetrominos::randomTrack(const std::string &exclude) {
    const std::string tracks[] = {"A", "B", "C"};
    std::vector<std::string> choices;
    for (const auto &t : tracks)
        if (t != exclude) choices.push_back(t);
    return choices[static_cast<size_t>(Random::getInteger(0, static_cast<int>(choices.size()) - 1))];
}

void Tetrominos::playStartingMusic() {
    switch (SoundEngine::getSoundtrackMode()) {
        case SoundtrackMode::Cycle: SoundEngine::playMusic("A"); break;
        case SoundtrackMode::Random: SoundEngine::playMusic(randomTrack()); break;
        case SoundtrackMode::TrackA: SoundEngine::playMusic("A"); break;
        case SoundtrackMode::TrackB: SoundEngine::playMusic("B"); break;
        case SoundtrackMode::TrackC: SoundEngine::playMusic("C"); break;
    }
}
