#include "Tetris.h"

#include <iostream>

#include "Timer.h"
#include "Menu.h"
#include "Platform.h"
#include "SoundEngine.h"
#include "rlutil.h"

Tetris::Tetris(Menu &pauseMenu, Menu &gameOverMenu)
    : _controller(Timer::instance()), _pauseMenu(pauseMenu), _gameOverMenu(gameOverMenu)
{
    _state.loadHighscore();
    _state.loadOptions();
}

Tetris::~Tetris() = default;

void Tetris::start() {
    _renderer.configure(_state.previewCount(), _state.holdEnabled());
    _controller.configurePolicies(_state.mode());
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
            handlePause();
            break;
        case StepResult::GameOver:
            handleGameOver();
            break;
    }
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
        _controller.reset(_state);
        _renderer.configure(_state.previewCount(), _state.holdEnabled());
        _renderer.invalidate();
        _renderer.render(_state);
        _state.clearDirty();
        SoundEngine::playMusic("A");
        return;
    }

    if (selected == "Main Menu") {
        _state.saveHighscore();
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
    if (_state.hasBetterHighscore())
        _state.setPlayerName(promptPlayerName());
    _state.saveHighscore();

    const OptionChoice choices = _gameOverMenu.open(_state.hasBetterHighscore());
    const auto& selected = choices.options[choices.selected];

    if (selected == "Main Menu") {
        _backToMenu = true;
        return;
    }

    _controller.reset(_state);
    _renderer.configure(_state.previewCount(), _state.holdEnabled());
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

std::string Tetris::promptPlayerName() {
    constexpr int kMaxName = 10;
    constexpr int kInterior = 20;

    Panel panel(kInterior);
    panel.addRow("NEW HIGH SCORE!", Align::CENTER);
    panel.addSeparator();
    panel.addRow("Enter your name:", Align::CENTER);
    size_t nameRow = panel.addRow("__________", Align::CENTER);

    constexpr int windowWidth = 80;
    constexpr int windowHeight = 28;
    int w = panel.width();
    int h = panel.height();
    int px = Platform::offsetX() + (windowWidth / 2) - (w / 2);
    int py = Platform::offsetY() + (windowHeight / 2) - (h / 2);
    panel.setPosition(px, py);

    std::string name;
    Platform::flushInput();

    while (true) {
        // Build display: typed chars + underscores for remaining slots
        std::string display = name;
        for (int i = static_cast<int>(name.size()); i < kMaxName; i++)
            display += '_';
        panel.setCell(nameRow, 0, display);

        if (!Platform::isTerminalTooSmall()) {
            panel.render();
            std::cout << std::flush;
        }

        int key = Platform::getKey();

        if (key == rlutil::KEY_ENTER) {
            break;
        } else if (key == rlutil::KEY_ESCAPE) {
            name.clear();
            break;
        } else if (key == 8 || key == 127) {
            // Backspace or DEL
            if (!name.empty())
                name.pop_back();
        } else if (key >= 32 && key <= 126 && static_cast<int>(name.size()) < kMaxName) {
            name += static_cast<char>(key);
        }

        if (Platform::wasResized()) {
            if (!Platform::isTerminalTooSmall()) {
                if (Menu::onResize)
                    Menu::onResize();
                px = Platform::offsetX() + (windowWidth / 2) - (w / 2);
                py = Platform::offsetY() + (windowHeight / 2) - (h / 2);
                panel.setPosition(px, py);
                panel.invalidate();
            }
            continue;
        }

        if (Menu::shouldExitGame && Menu::shouldExitGame())
            break;
    }

    panel.clear();
    return name;
}
