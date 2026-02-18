#pragma once

#include <memory>

#include "GameEngine.h"

class GameMenus;
class HighScoreDisplay;
class HelpDisplay;
class Tetrominos;
struct InputSnapshot;

class TetrominosGame : public GameEngine {
protected:
    void onInit() override;
    void onFrame(double dt) override;
    void onCleanup() override;
    void onResize() override;
    void onTerminalTooSmall() override;
    void onTerminalRestored() override;

public:
    TetrominosGame();
    ~TetrominosGame() override;
    TetrominosGame(const TetrominosGame &) = delete;
    TetrominosGame &operator=(const TetrominosGame &) = delete;

private:
    enum class Screen { MainMenu, Playing };
    static void bindDefaultKeys();
    static InputSnapshot pollInputSnapshot();

    std::unique_ptr<GameMenus> _menus;
    std::unique_ptr<HighScoreDisplay> _highScores;
    std::unique_ptr<HelpDisplay> _help;
    std::unique_ptr<Tetrominos> _game;
    Screen _screen{Screen::MainMenu};
};
