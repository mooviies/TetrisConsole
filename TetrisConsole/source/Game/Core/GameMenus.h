#pragma once

#include <string>
#include <vector>

#include "Menu.h"

class Tetrominos;
class HighScoreDisplay;
class HelpDisplay;

class GameMenus {
public:
    GameMenus();
    ~GameMenus();

    void configure(Tetrominos &game, HighScoreDisplay &highScores, HelpDisplay &help);

    Menu &mainMenu() { return _main; }
    Menu &pauseMenu() { return _pause; }
    Menu &gameOverMenu() { return _gameOver; }

private:
    void syncSoundToMenu(Menu &menu);
    void applySoundFromMenu(Menu &menu);

    Tetrominos *_game = nullptr;

    std::vector<std::string> _volumeValues;

    Menu _main, _newGame, _options;
    Menu _pause, _pauseSound;
    Menu _restartConfirm, _backToMenuConfirm, _quit;
    Menu _gameOver;
};
