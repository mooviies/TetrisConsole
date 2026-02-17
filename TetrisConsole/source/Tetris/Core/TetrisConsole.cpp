#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>

#include "Tetris.h"
#include "Constants.h"
#include "HelpDisplay.h"
#include "HighScoreDisplay.h"
#include "Input.h"
#include "InputSnapshot.h"
#include "SoundEngine.h"
#include "Utility.h"
#include "GameRenderer.h"
#include "Menu.h"
#include "Platform.h"
#include "rlutil.h"

#ifdef TETRIS_DEBUG
#include "TestRunner.h"
#endif

using namespace std;

int main() {
    Platform::initConsole();
    Input::init(static_cast<int>(Action::Count));

    // Default key bindings
    auto A = [](Action a) { return static_cast<int>(a); };

    Input::bind(A(Action::Left), KeyCode::ArrowLeft);
    Input::bind(A(Action::Left), KeyCode(static_cast<int>('A')));
    Input::bind(A(Action::Left), KeyCode::Numpad4);

    Input::bind(A(Action::Right), KeyCode::ArrowRight);
    Input::bind(A(Action::Right), KeyCode(static_cast<int>('D')));
    Input::bind(A(Action::Right), KeyCode::Numpad6);

    Input::bind(A(Action::SoftDrop), KeyCode::ArrowDown);
    Input::bind(A(Action::SoftDrop), KeyCode(static_cast<int>('S')));
    Input::bind(A(Action::SoftDrop), KeyCode::Numpad2);

    Input::bind(A(Action::HardDrop), KeyCode(static_cast<int>(' ')));
    Input::bind(A(Action::HardDrop), KeyCode::Numpad8);

    Input::bind(A(Action::RotateCW), KeyCode::ArrowUp);
    Input::bind(A(Action::RotateCW), KeyCode(static_cast<int>('X')));
    Input::bind(A(Action::RotateCW), KeyCode::Numpad1);
    Input::bind(A(Action::RotateCW), KeyCode::Numpad5);
    Input::bind(A(Action::RotateCW), KeyCode::Numpad9);

    Input::bind(A(Action::RotateCCW), KeyCode(static_cast<int>('Z')));
    Input::bind(A(Action::RotateCCW), KeyCode::Numpad3);
    Input::bind(A(Action::RotateCCW), KeyCode::Numpad7);

    Input::bind(A(Action::Hold), KeyCode(static_cast<int>('C')));
    Input::bind(A(Action::Hold), KeyCode::Numpad0);

    Input::bind(A(Action::Pause), KeyCode::Escape);
    Input::bind(A(Action::Pause), KeyCode::F1);

    Input::bind(A(Action::Select), KeyCode::Enter);

    if (!SoundEngine::init()) {
        Input::cleanup();
        Platform::cleanupConsole();
        return 1;
    }

    while (Platform::isTerminalTooSmall()) {
        Platform::showResizePrompt();
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    GameRenderer::renderTitle("A classic in console!");

    vector<string> levels;
    for (int i = MIN_LEVEL; i <= MAX_LEVEL; i++)
        levels.push_back(Utility::valueToString(i, 2));

    vector<string> lockDownMode;
    lockDownMode.emplace_back("Extended");
    lockDownMode.emplace_back("Infinite");
    lockDownMode.emplace_back("Classic");

    vector<string> variants;
    variants.emplace_back("Marathon");
    variants.emplace_back("Sprint");
    variants.emplace_back("Ultra");

    Menu main("MAIN MENU");
    Menu newGame("NEW GAME");
    Menu options("OPTIONS");
    Menu pause("PAUSE");
    Menu pauseSound("SOUND");
    Menu restartConfirm("Restart game?");
    Menu backToMenuConfirm("Back to menu?");
    Menu quit("Quit game?");
    Menu gameOver("GAME OVER", "New High Score!");

    HighScoreDisplay highScores;
    HelpDisplay help;
    Tetris tetris(pause, gameOver, highScores);

    Menu::shouldExitGame = [&tetris]() { return tetris.doExit(); }; // NOLINT(dangling) — both live in main
    Menu::onResize = []() { GameRenderer::renderTitle("A classic in console!"); };

    // --- New Game menu (Level + Start) ---
    newGame.addOptionCloseAllMenu("Start", [&tetris](const OptionChoice &oc) {
        int level = 1;
        try {
            level = stoi(oc.values.at("Level"));
        } catch (...) {}
        tetris.setStartingLevel(level);

        auto v = GameVariant::Marathon;
        if (oc.values.at("Variant") == "Sprint")
            v = GameVariant::Sprint;
        else if (oc.values.at("Variant") == "Ultra")
            v = GameVariant::Ultra;
        tetris.setVariant(v);

        tetris.saveOptions();
    });
    newGame.addOptionWithValues("Level", levels);
    newGame.addOptionWithValues("Variant", variants);

    newGame.setOptionHint("Start", "Start a new game with the selected settings.");
    newGame.setOptionHint("Level", "Starting speed. Higher levels are faster.");
    newGame.setOptionValueHint("Variant", "Marathon", "Reach over level 15 with the highest score.");
    newGame.setOptionValueHint("Variant", "Sprint", "Clear 40 lines in the shortest amount of time.");
    newGame.setOptionValueHint("Variant", "Ultra", "Get the highest score in 2 minutes.");

    // --- Options menu (Lock Down, Ghost, Hold, Preview, Reset Defaults, Back) ---
    vector<string> ghostValues = {"On", "Off"};
    vector<string> holdValues = {"On", "Off"};
    vector<string> previewValues;
    for (int i = 0; i <= 6; i++)
        previewValues.push_back(Utility::valueToString(i, 2));

    vector<string> volumeValues;
    for (int i = 0; i <= 10; i++)
        volumeValues.emplace_back(string(static_cast<size_t>(i), '#') + string(static_cast<size_t>(10 - i), '-'));

    vector<string> soundtrackValues = {"Cycle", "Random", "A", "B", "C"};

    options.addOptionWithValues("Lock Down", lockDownMode);
    options.addOptionWithValues("Ghost Piece", ghostValues);
    options.addOptionWithValues("Hold Piece", holdValues);
    options.addOptionWithValues("Preview", previewValues);
    options.addOptionWithValues("Music", volumeValues);
    options.addOptionWithValues("Effects", volumeValues);
    options.addOptionWithValues("Soundtrack", soundtrackValues);
    options.addOption("Reset Defaults", [&options, &volumeValues](const OptionChoice &) {
        options.setValueChoice("Lock Down", "Extended");
        options.setValueChoice("Ghost Piece", "On");
        options.setValueChoice("Hold Piece", "On");
        options.setValueChoice("Preview", Utility::valueToString(6, 2));
        options.setValueChoice("Music", volumeValues[5]);
        options.setValueChoice("Effects", volumeValues[5]);
        options.setValueChoice("Soundtrack", "Cycle");
    });
    options.addOptionClose("Back");

    options.setOptionValueHint("Lock Down", "Extended", "Locks 0.5s after landing. 15 move resets.");
    options.setOptionValueHint("Lock Down", "Infinite", "Locks 0.5s after landing. Unlimited resets.");
    options.setOptionValueHint("Lock Down", "Classic", "Locks immediately when the piece can't fall.");
    options.setOptionValueHint("Ghost Piece", "On", "Shows where the current piece will land.");
    options.setOptionValueHint("Ghost Piece", "Off", "Hides the landing preview.");
    options.setOptionValueHint("Hold Piece", "On", "Swap the current piece once per drop.");
    options.setOptionValueHint("Hold Piece", "Off", "Hold piece is disabled.");
    for (int i = 6; i >= 0; i--) {
        string val = Utility::valueToString(i, 2);
        string hint;
        if (i == 0)
            hint = "Hides the next piece queue.";
        else if (i == 1)
            hint = "Shows the next piece.";
        else
            hint = "Shows the next " + to_string(i) + " pieces.";
        options.setOptionValueHint("Preview", val, hint);
    }
    options.setOptionHint("Music", "Adjust the music volume.");
    options.setOptionHint("Effects", "Adjust the sound effects volume.");
    options.setOptionValueHint("Soundtrack", "Cycle", "Play tracks A, B, C in order, then repeat.");
    options.setOptionValueHint("Soundtrack", "Random", "Play a random track each time.");
    options.setOptionValueHint("Soundtrack", "A", "Always play track A.");
    options.setOptionValueHint("Soundtrack", "B", "Always play track B.");
    options.setOptionValueHint("Soundtrack", "C", "Always play track C.");
    options.setOptionHint("Reset Defaults", "Restore all options to their default values.");
    options.setOptionHint("Back", "Return to the main menu.");

    // --- Main menu ---
    main.addOption("New Game", &newGame, [&]() {
        newGame.setValueChoice("Level", Utility::valueToString(tetris.startingLevel(), 2));
        string varStr = "Marathon";
        if (tetris.variant() == GameVariant::Sprint)
            varStr = "Sprint";
        else if (tetris.variant() == GameVariant::Ultra)
            varStr = "Ultra";
        newGame.setValueChoice("Variant", varStr);
    });
    main.addOptionAction("Options", [&]() {
        // Sync menu values from current tetris state
        string modeStr = "Extended";
        if (tetris.mode() == LockDownMode::Classic)
            modeStr = "Classic";
        else if (tetris.mode() == LockDownMode::ExtendedInfinity)
            modeStr = "Infinite";
        options.setValueChoice("Lock Down", modeStr);
        options.setValueChoice("Ghost Piece", tetris.ghostEnabled() ? "On" : "Off");
        options.setValueChoice("Hold Piece", tetris.holdEnabled() ? "On" : "Off");
        options.setValueChoice("Preview", Utility::valueToString(tetris.previewCount(), 2));

        int musicStep = clamp(static_cast<int>(lroundf(SoundEngine::getMusicVolume() * 50)), 0, 10);
        options.setValueChoice("Music", volumeValues[static_cast<size_t>(musicStep)]);

        int effectStep = clamp(static_cast<int>(lroundf(SoundEngine::getEffectVolume() * 10)), 0, 10);
        options.setValueChoice("Effects", volumeValues[static_cast<size_t>(effectStep)]);

        auto stMode = SoundEngine::getSoundtrackMode();
        string stStr = "Cycle";
        if (stMode == SoundtrackMode::Random) stStr = "Random";
        else if (stMode == SoundtrackMode::TrackA) stStr = "A";
        else if (stMode == SoundtrackMode::TrackB) stStr = "B";
        else if (stMode == SoundtrackMode::TrackC) stStr = "C";
        options.setValueChoice("Soundtrack", stStr);

        options.open(false, true);

        // Apply values back to tetris
        auto values = options.generateValues();
        auto mode = LockDownMode::Extended;
        if (values["Lock Down"] == "Classic")
            mode = LockDownMode::Classic;
        else if (values["Lock Down"] == "Infinite")
            mode = LockDownMode::ExtendedInfinity;
        tetris.setLockDownMode(mode);
        tetris.setGhostEnabled(values["Ghost Piece"] != "Off");
        tetris.setHoldEnabled(values["Hold Piece"] != "Off");
        int preview = 6;
        try {
            preview = stoi(values["Preview"]);
        } catch (...) {}
        tetris.setPreviewCount(preview);

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

        tetris.saveOptions();
    });
    main.addOptionAction("High Scores", [&]() { highScores.open(tetris.allHighscores(), tetris.variant()); });
    main.addOptionAction("Help", [&]() { help.open(); });
#ifdef TETRIS_DEBUG
    main.addOptionAction("Test", [&]() {
        rlutil::cls();
        GameRenderer::renderTitle("Test Runner");
        TestRunner runner;
        runner.run();
        rlutil::cls();
        GameRenderer::renderTitle("A classic in console!");
    });
#endif
    main.addOption("Exit", &quit);

    // --- Pause Sound menu ---
    pauseSound.addOptionWithValues("Music", volumeValues);
    pauseSound.addOptionWithValues("Effects", volumeValues);
    pauseSound.addOptionWithValues("Soundtrack", soundtrackValues);
    pauseSound.addOptionClose("Back");

    pauseSound.setOptionHint("Music", "Adjust the music volume.");
    pauseSound.setOptionHint("Effects", "Adjust the sound effects volume.");
    pauseSound.setOptionValueHint("Soundtrack", "Cycle", "Play tracks A, B, C in order, then repeat.");
    pauseSound.setOptionValueHint("Soundtrack", "Random", "Play a random track each time.");
    pauseSound.setOptionValueHint("Soundtrack", "A", "Always play track A.");
    pauseSound.setOptionValueHint("Soundtrack", "B", "Always play track B.");
    pauseSound.setOptionValueHint("Soundtrack", "C", "Always play track C.");
    pauseSound.setOptionHint("Back", "Return to the pause menu.");

    // --- Pause menu ---
    pause.addOptionClose("Resume");
    pause.addOption("Restart", &restartConfirm);
    pause.addOptionAction("Options", [&]() {
        int musicStep = clamp(static_cast<int>(lroundf(SoundEngine::getMusicVolume() * 50)), 0, 10);
        pauseSound.setValueChoice("Music", volumeValues[static_cast<size_t>(musicStep)]);

        int effectStep = clamp(static_cast<int>(lroundf(SoundEngine::getEffectVolume() * 10)), 0, 10);
        pauseSound.setValueChoice("Effects", volumeValues[static_cast<size_t>(effectStep)]);

        auto stMode = SoundEngine::getSoundtrackMode();
        string stStr = "Cycle";
        if (stMode == SoundtrackMode::Random) stStr = "Random";
        else if (stMode == SoundtrackMode::TrackA) stStr = "A";
        else if (stMode == SoundtrackMode::TrackB) stStr = "B";
        else if (stMode == SoundtrackMode::TrackC) stStr = "C";
        pauseSound.setValueChoice("Soundtrack", stStr);

        pauseSound.open(false, true);

        auto values = pauseSound.generateValues();

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

        tetris.saveOptions();
    });
    pause.addOption("Main Menu", &backToMenuConfirm);
    pause.addOption("Exit Game", &quit);

    restartConfirm.addOptionCloseAllMenu("Yes");
    restartConfirm.addOptionClose("No");

    backToMenuConfirm.addOptionCloseAllMenu("Yes");
    backToMenuConfirm.addOptionClose("No");

    quit.addOption("Yes", [&tetris](const OptionChoice &) { tetris.exit(); });
    quit.addOptionClose("No");

    // --- Game Over menu ---
    gameOver.addOptionClose("Retry");
    gameOver.addOptionClose("Main Menu");
    gameOver.addOption("Exit Game", &quit);

    while (true) {
        main.open();
        if (tetris.doExit()) break;
        tetris.start();

        bool wasTooSmall = false;
        while (!tetris.doExit() && !tetris.backToMenu()) {
            const auto frameStart = chrono::steady_clock::now();

            Input::pollKeys();
            if (Platform::wasResized()) tetris.redraw();
            if (Platform::isTerminalTooSmall()) {
                if (!wasTooSmall) {
                    tetris.pauseGameTimer();
                    wasTooSmall = true;
                }
                this_thread::sleep_for(chrono::milliseconds(50));
                continue;
            }
            if (wasTooSmall) {
                tetris.resumeGameTimer();
                wasTooSmall = false;
            }

            InputSnapshot snapshot;
            snapshot.left = Input::action(A(Action::Left));
            snapshot.right = Input::action(A(Action::Right));
            snapshot.softDrop = Input::action(A(Action::SoftDrop));
            snapshot.hardDrop = Input::action(A(Action::HardDrop));
            snapshot.rotateCW = Input::action(A(Action::RotateCW));
            snapshot.rotateCCW = Input::action(A(Action::RotateCCW));
            snapshot.hold = Input::action(A(Action::Hold));
            snapshot.pause = Input::action(A(Action::Pause));

            tetris.step(snapshot);
            tetris.render();

            const auto elapsed = chrono::steady_clock::now() - frameStart;
            if (const auto sleepTime = chrono::milliseconds(16) - elapsed; sleepTime > chrono::milliseconds(0))
                this_thread::sleep_for(sleepTime);
        }
        if (tetris.doExit()) break;
        tetris.clearBackToMenu();
        rlutil::cls();
        GameRenderer::renderTitle("A classic in console!");
    }

    Menu::shouldExitGame = nullptr;
    Menu::onResize = nullptr;
    SoundEngine::cleanup();
    Input::cleanup();
    Platform::cleanupConsole();

    return 0;
}
