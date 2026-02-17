#include "GameMenus.h"

#include <algorithm>
#include <cmath>
#include <string>

#include "Tetrominos.h"
#include "Constants.h"
#include "GameRenderer.h"
#include "HighScoreDisplay.h"
#include "HelpDisplay.h"
#include "SoundEngine.h"
#include "Utility.h"
#include "rlutil.h"

#ifdef GAME_DEBUG
#include "TestRunner.h"
#endif

using namespace std;

GameMenus::GameMenus()
    : _main("MAIN MENU")
    , _newGame("NEW GAME")
    , _options("OPTIONS")
    , _pause("PAUSE")
    , _pauseSound("SOUND")
    , _restartConfirm("Restart game?")
    , _backToMenuConfirm("Back to menu?")
    , _quit("Quit game?")
    , _gameOver("GAME OVER", "New High Score!")
{
    for (int i = 0; i <= 10; i++)
        _volumeValues.emplace_back(string(static_cast<size_t>(i), '#') + string(static_cast<size_t>(10 - i), '-'));
}

GameMenus::~GameMenus() {
    Menu::shouldExitGame = nullptr;
    Menu::onResize = nullptr;
}

void GameMenus::syncSoundToMenu(Menu &menu) {
    int musicStep = clamp(static_cast<int>(lroundf(SoundEngine::getMusicVolume() * 50)), 0, 10);
    menu.setValueChoice("Music", _volumeValues[static_cast<size_t>(musicStep)]);

    int effectStep = clamp(static_cast<int>(lroundf(SoundEngine::getEffectVolume() * 10)), 0, 10);
    menu.setValueChoice("Effects", _volumeValues[static_cast<size_t>(effectStep)]);

    auto stMode = SoundEngine::getSoundtrackMode();
    string stStr = "Cycle";
    if (stMode == SoundtrackMode::Random) stStr = "Random";
    else if (stMode == SoundtrackMode::TrackA) stStr = "A";
    else if (stMode == SoundtrackMode::TrackB) stStr = "B";
    else if (stMode == SoundtrackMode::TrackC) stStr = "C";
    menu.setValueChoice("Soundtrack", stStr);
}

void GameMenus::applySoundFromMenu(Menu &menu) {
    auto values = menu.generateValues();

    auto hashes = count(values["Music"].begin(), values["Music"].end(), '#');
    SoundEngine::setMusicVolume(static_cast<float>(hashes) * 0.02f);

    hashes = count(values["Effects"].begin(), values["Effects"].end(), '#');
    SoundEngine::setEffectVolume(static_cast<float>(hashes) * 0.1f);

    const auto &st = values["Soundtrack"];
    if (st == "Random") SoundEngine::setSoundtrackMode(SoundtrackMode::Random);
    else if (st == "A") SoundEngine::setSoundtrackMode(SoundtrackMode::TrackA);
    else if (st == "B") SoundEngine::setSoundtrackMode(SoundtrackMode::TrackB);
    else if (st == "C") SoundEngine::setSoundtrackMode(SoundtrackMode::TrackC);
    else SoundEngine::setSoundtrackMode(SoundtrackMode::Cycle);
}

void GameMenus::configure(Tetrominos &game, HighScoreDisplay &highScores, HelpDisplay &help) {
    _game = &game;

    Menu::shouldExitGame = [this]() { return _game->doExit(); };
    Menu::onResize = []() { GameRenderer::renderTitle("A classic in console!"); };

    // --- Value vectors (local to configure, not captured by reference) ---
    vector<string> levels;
    for (int i = MIN_LEVEL; i <= MAX_LEVEL; i++)
        levels.push_back(Utility::valueToString(i, 2));

    vector<string> lockDownModes;
    lockDownModes.emplace_back("Extended");
    lockDownModes.emplace_back("Infinite");
    lockDownModes.emplace_back("Classic");

    vector<string> variants;
    variants.emplace_back("Marathon");
    variants.emplace_back("Sprint");
    variants.emplace_back("Ultra");

    vector<string> ghostValues = {"On", "Off"};
    vector<string> holdValues = {"On", "Off"};

    vector<string> previewValues;
    for (int i = 0; i <= 6; i++)
        previewValues.push_back(Utility::valueToString(i, 2));

    vector<string> soundtrackValues = {"Cycle", "Random", "A", "B", "C"};

    // --- New Game menu ---
    _newGame.addOptionCloseAllMenu("Start", [this](const OptionChoice &oc) {
        int level = 1;
        try {
            level = stoi(oc.values.at("Level"));
        } catch (...) {}
        _game->setStartingLevel(level);

        auto v = GameVariant::Marathon;
        if (oc.values.at("Variant") == "Sprint")
            v = GameVariant::Sprint;
        else if (oc.values.at("Variant") == "Ultra")
            v = GameVariant::Ultra;
        _game->setVariant(v);

        _game->saveOptions();
    });
    _newGame.addOptionWithValues("Level", levels);
    _newGame.addOptionWithValues("Variant", variants);

    _newGame.setOptionHint("Start", "Start a new game with the selected settings.");
    _newGame.setOptionHint("Level", "Starting speed. Higher levels are faster.");
    _newGame.setOptionValueHint("Variant", "Marathon", "Reach over level 15 with the highest score.");
    _newGame.setOptionValueHint("Variant", "Sprint", "Clear 40 lines in the shortest amount of time.");
    _newGame.setOptionValueHint("Variant", "Ultra", "Get the highest score in 2 minutes.");

    // --- Options menu ---
    _options.addOptionWithValues("Lock Down", lockDownModes);
    _options.addOptionWithValues("Ghost Piece", ghostValues);
    _options.addOptionWithValues("Hold Piece", holdValues);
    _options.addOptionWithValues("Preview", previewValues);
    _options.addOptionWithValues("Music", _volumeValues);
    _options.addOptionWithValues("Effects", _volumeValues);
    _options.addOptionWithValues("Soundtrack", soundtrackValues);
    _options.addOption("Reset Defaults", [this](const OptionChoice &) {
        _options.setValueChoice("Lock Down", "Extended");
        _options.setValueChoice("Ghost Piece", "On");
        _options.setValueChoice("Hold Piece", "On");
        _options.setValueChoice("Preview", Utility::valueToString(6, 2));
        _options.setValueChoice("Music", _volumeValues[5]);
        _options.setValueChoice("Effects", _volumeValues[5]);
        _options.setValueChoice("Soundtrack", "Cycle");
    });
    _options.addOptionClose("Back");

    _options.setOptionValueHint("Lock Down", "Extended", "Locks 0.5s after landing. 15 move resets.");
    _options.setOptionValueHint("Lock Down", "Infinite", "Locks 0.5s after landing. Unlimited resets.");
    _options.setOptionValueHint("Lock Down", "Classic", "Locks immediately when the piece can't fall.");
    _options.setOptionValueHint("Ghost Piece", "On", "Shows where the current piece will land.");
    _options.setOptionValueHint("Ghost Piece", "Off", "Hides the landing preview.");
    _options.setOptionValueHint("Hold Piece", "On", "Swap the current piece once per drop.");
    _options.setOptionValueHint("Hold Piece", "Off", "Hold piece is disabled.");
    for (int i = 6; i >= 0; i--) {
        string val = Utility::valueToString(i, 2);
        string hint;
        if (i == 0)
            hint = "Hides the next piece queue.";
        else if (i == 1)
            hint = "Shows the next piece.";
        else
            hint = "Shows the next " + to_string(i) + " pieces.";
        _options.setOptionValueHint("Preview", val, hint);
    }
    _options.setOptionHint("Music", "Adjust the music volume.");
    _options.setOptionHint("Effects", "Adjust the sound effects volume.");
    _options.setOptionValueHint("Soundtrack", "Cycle", "Play tracks A, B, C in order, then repeat.");
    _options.setOptionValueHint("Soundtrack", "Random", "Play a random track each time.");
    _options.setOptionValueHint("Soundtrack", "A", "Always play track A.");
    _options.setOptionValueHint("Soundtrack", "B", "Always play track B.");
    _options.setOptionValueHint("Soundtrack", "C", "Always play track C.");
    _options.setOptionHint("Reset Defaults", "Restore all options to their default values.");
    _options.setOptionHint("Back", "Return to the main menu.");

    // --- Main menu ---
    _main.addOption("New Game", &_newGame, [this]() {
        _newGame.setValueChoice("Level", Utility::valueToString(_game->startingLevel(), 2));
        string varStr = "Marathon";
        if (_game->variant() == GameVariant::Sprint)
            varStr = "Sprint";
        else if (_game->variant() == GameVariant::Ultra)
            varStr = "Ultra";
        _newGame.setValueChoice("Variant", varStr);
    });
    _main.addOptionAction("Options", [this]() {
        string modeStr = "Extended";
        if (_game->mode() == LockDownMode::Classic)
            modeStr = "Classic";
        else if (_game->mode() == LockDownMode::ExtendedInfinity)
            modeStr = "Infinite";
        _options.setValueChoice("Lock Down", modeStr);
        _options.setValueChoice("Ghost Piece", _game->ghostEnabled() ? "On" : "Off");
        _options.setValueChoice("Hold Piece", _game->holdEnabled() ? "On" : "Off");
        _options.setValueChoice("Preview", Utility::valueToString(_game->previewCount(), 2));

        syncSoundToMenu(_options);

        _options.open(false, true);

        auto values = _options.generateValues();
        auto mode = LockDownMode::Extended;
        if (values["Lock Down"] == "Classic")
            mode = LockDownMode::Classic;
        else if (values["Lock Down"] == "Infinite")
            mode = LockDownMode::ExtendedInfinity;
        _game->setLockDownMode(mode);
        _game->setGhostEnabled(values["Ghost Piece"] != "Off");
        _game->setHoldEnabled(values["Hold Piece"] != "Off");
        int preview = 6;
        try {
            preview = stoi(values["Preview"]);
        } catch (...) {}
        _game->setPreviewCount(preview);

        applySoundFromMenu(_options);

        _game->saveOptions();
    });
    _main.addOptionAction("High Scores", [this, &highScores]() {
        highScores.open(_game->allHighscores(), _game->variant());
    });
    _main.addOptionAction("Help", [&help]() { help.open(); });
#ifdef GAME_DEBUG
    _main.addOptionAction("Test", []() {
        rlutil::cls();
        GameRenderer::renderTitle("Test Runner");
        TestRunner runner;
        runner.run();
        rlutil::cls();
        GameRenderer::renderTitle("A classic in console!");
    });
#endif
    _main.addOption("Exit", &_quit);

    // --- Pause Sound menu ---
    _pauseSound.addOptionWithValues("Music", _volumeValues);
    _pauseSound.addOptionWithValues("Effects", _volumeValues);
    _pauseSound.addOptionWithValues("Soundtrack", soundtrackValues);
    _pauseSound.addOptionClose("Back");

    _pauseSound.setOptionHint("Music", "Adjust the music volume.");
    _pauseSound.setOptionHint("Effects", "Adjust the sound effects volume.");
    _pauseSound.setOptionValueHint("Soundtrack", "Cycle", "Play tracks A, B, C in order, then repeat.");
    _pauseSound.setOptionValueHint("Soundtrack", "Random", "Play a random track each time.");
    _pauseSound.setOptionValueHint("Soundtrack", "A", "Always play track A.");
    _pauseSound.setOptionValueHint("Soundtrack", "B", "Always play track B.");
    _pauseSound.setOptionValueHint("Soundtrack", "C", "Always play track C.");
    _pauseSound.setOptionHint("Back", "Return to the pause menu.");

    // --- Pause menu ---
    _pause.addOptionClose("Resume");
    _pause.addOption("Restart", &_restartConfirm);
    _pause.addOptionAction("Options", [this]() {
        syncSoundToMenu(_pauseSound);

        _pauseSound.open(false, true);

        applySoundFromMenu(_pauseSound);

        _game->saveOptions();
    });
    _pause.addOption("Main Menu", &_backToMenuConfirm);
    _pause.addOption("Exit Game", &_quit);

    _restartConfirm.addOptionCloseAllMenu("Yes");
    _restartConfirm.addOptionClose("No");

    _backToMenuConfirm.addOptionCloseAllMenu("Yes");
    _backToMenuConfirm.addOptionClose("No");

    _quit.addOption("Yes", [this](const OptionChoice &) { _game->exit(); });
    _quit.addOptionClose("No");

    // --- Game Over menu ---
    _gameOver.addOptionClose("Retry");
    _gameOver.addOptionClose("Main Menu");
    _gameOver.addOption("Exit Game", &_quit);
}
