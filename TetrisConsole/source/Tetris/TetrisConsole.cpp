#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "Tetris.h"
#include "Constants.h"
#include "HighScoreDisplay.h"
#include "Input.h"
#include "SoundEngine.h"
#include "Utility.h"
#include "GameRenderer.h"
#include "Menu.h"
#include "Platform.h"
#include "rlutil.h"

using namespace std;

int main() {
    Platform::initConsole();
    Input::init();

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
    for (int i = 1; i <= 15; i++)
        levels.push_back(Utility::valueToString(i, 2));

    vector<string> lockDownMode;
    lockDownMode.emplace_back("Extended");
    lockDownMode.emplace_back("Infinite");
    lockDownMode.emplace_back("Classic");

    Menu main("MAIN MENU");
    Menu newGame("NEW GAME");
    Menu options("OPTIONS");
    Menu pause("PAUSE");
    Menu restartConfirm("Restart game?");
    Menu backToMenuConfirm("Back to menu?");
    Menu quit("Quit game?");
    Menu gameOver("GAME OVER", "New High Score!");

    HighScoreDisplay highScores;
    Tetris tetris(pause, gameOver, highScores);

    Menu::shouldExitGame = [&tetris]() { return tetris.doExit(); };
    Menu::onResize = []() { GameRenderer::renderTitle("A classic in console!"); };

    // --- New Game menu (Level + Start) ---
    newGame.addOptionCloseAllMenu("Start", [&tetris](OptionChoice oc) {
        int level = 1;
        try { level = stoi(oc.values["Level"]); } catch (...) {}
        tetris.setStartingLevel(level);
        tetris.saveOptions();
    });
    newGame.addOptionWithValues("Level", levels);

    newGame.setOptionHint("Start", "Start a new game with the selected settings.");
    newGame.setOptionHint("Level", "Starting speed. Higher levels are faster.");

    // --- Options menu (Lock Down, Ghost, Hold, Preview, Reset Defaults, Back) ---
    vector<string> ghostValues = {"On", "Off"};
    vector<string> holdValues = {"On", "Off"};
    vector<string> previewValues;
    for (int i = 0; i <= NEXT_PIECE_QUEUE_SIZE; i++)
        previewValues.push_back(Utility::valueToString(i, 2));

    options.addOptionWithValues("Lock Down", lockDownMode);
    options.addOptionWithValues("Ghost Piece", ghostValues);
    options.addOptionWithValues("Hold Piece", holdValues);
    options.addOptionWithValues("Preview", previewValues);
    options.addOption("Reset Defaults", [&options](OptionChoice) {
        options.setValueChoice("Lock Down", "Extended");
        options.setValueChoice("Ghost Piece", "On");
        options.setValueChoice("Hold Piece", "On");
        options.setValueChoice("Preview", Utility::valueToString(NEXT_PIECE_QUEUE_SIZE, 2));
    });
    options.addOptionClose("Back");

    options.setOptionValueHint("Lock Down", "Extended", "Locks 0.5s after landing. 15 move resets.");
    options.setOptionValueHint("Lock Down", "Infinite", "Locks 0.5s after landing. Unlimited resets.");
    options.setOptionValueHint("Lock Down", "Classic", "Locks immediately when the piece can't fall.");
    options.setOptionValueHint("Ghost Piece", "On", "Shows where the current piece will land.");
    options.setOptionValueHint("Ghost Piece", "Off", "Hides the landing preview.");
    options.setOptionValueHint("Hold Piece", "On", "Swap the current piece once per drop.");
    options.setOptionValueHint("Hold Piece", "Off", "Hold piece is disabled.");
    for (int i = NEXT_PIECE_QUEUE_SIZE; i >= 0; i--) {
        string val = Utility::valueToString(i, 2);
        string hint;
        if (i == 0) hint = "Hides the next piece queue.";
        else if (i == 1) hint = "Shows the next piece.";
        else hint = "Shows the next " + to_string(i) + " pieces.";
        options.setOptionValueHint("Preview", val, hint);
    }
    options.setOptionHint("Reset Defaults", "Restore all options to their default values.");
    options.setOptionHint("Back", "Return to the main menu.");

    // --- Main menu ---
    main.addOption("New Game", &newGame, [&]() {
        newGame.setValueChoice("Level", Utility::valueToString(tetris.startingLevel(), 2));
    });
    main.addOptionAction("Options", [&]() {
        // Sync menu values from current tetris state
        string modeStr = "Extended";
        if (tetris.mode() == MODE::CLASSIC) modeStr = "Classic";
        else if (tetris.mode() == MODE::EXTENDED_INFINITY) modeStr = "Infinite";
        options.setValueChoice("Lock Down", modeStr);
        options.setValueChoice("Ghost Piece", tetris.ghostEnabled() ? "On" : "Off");
        options.setValueChoice("Hold Piece", tetris.holdEnabled() ? "On" : "Off");
        options.setValueChoice("Preview", Utility::valueToString(tetris.previewCount(), 2));

        options.open(false, true);

        // Apply values back to tetris
        auto values = options.generateValues();
        MODE mode = MODE::CLASSIC;
        if (values["Lock Down"] == "Extended") mode = MODE::EXTENDED;
        else if (values["Lock Down"] == "Infinite") mode = MODE::EXTENDED_INFINITY;
        tetris.setMode(mode);
        tetris.setGhostEnabled(values["Ghost Piece"] != "Off");
        tetris.setHoldEnabled(values["Hold Piece"] != "Off");
        int preview = NEXT_PIECE_QUEUE_SIZE;
        try { preview = stoi(values["Preview"]); } catch (...) {}
        tetris.setPreviewCount(preview);
        tetris.saveOptions();
    });
    main.addOptionAction("High Scores", [&]() { highScores.open(tetris.highscores()); });
    main.addOption("Exit", &quit);

    // --- Pause menu ---
    pause.addOptionClose("Resume");
    pause.addOption("Restart", &restartConfirm);
    pause.addOption("Main Menu", &backToMenuConfirm);
    pause.addOption("Exit Game", &quit);

    restartConfirm.addOptionCloseAllMenu("Yes");
    restartConfirm.addOptionClose("No");

    backToMenuConfirm.addOptionCloseAllMenu("Yes");
    backToMenuConfirm.addOptionClose("No");

    quit.addOption("Yes", [&tetris](OptionChoice) { tetris.exit(); });
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
            if (Platform::wasResized())
                tetris.redraw();
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
            snapshot.left     = Input::left();
            snapshot.right    = Input::right();
            snapshot.softDrop = Input::softDrop();
            snapshot.hardDrop = Input::hardDrop();
            snapshot.rotateCW  = Input::rotateClockwise();
            snapshot.rotateCCW = Input::rotateCounterClockwise();
            snapshot.hold     = Input::hold();
            snapshot.pause    = Input::pause();
            snapshot.mute     = Input::mute();

            tetris.step(snapshot);
            tetris.render();

            const auto elapsed = chrono::steady_clock::now() - frameStart;
            if (const auto sleepTime = chrono::milliseconds(16) - elapsed;
                sleepTime > chrono::milliseconds(0))
                this_thread::sleep_for(sleepTime);
        }
        if (tetris.doExit()) break;
        tetris.clearBackToMenu();
        rlutil::cls();
        GameRenderer::renderTitle("A classic in console!");
    }

    SoundEngine::cleanup();
    Input::cleanup();
    Platform::cleanupConsole();

    return 0;
}
