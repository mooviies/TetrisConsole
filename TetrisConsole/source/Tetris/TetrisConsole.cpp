#include <algorithm>
#include <chrono>
#include <string>
#include <thread>

#include "Tetris.h"
#include "Input.h"
#include "SoundEngine.h"
#include "Utility.h"
#include "GameRenderer.h"
#include "Menu.h"
#include "Platform.h"

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
    Menu pause("PAUSE");
    Menu restartConfirm("Restart game?");
    Menu quit("Quit game?");
    Menu gameOver("GAME OVER", "New High Score!");

    Tetris tetris(pause, gameOver);

    Menu::shouldExitGame = [&tetris]() { return tetris.doExit(); };
    Menu::onResize = []() { GameRenderer::renderTitle("A classic in console!"); };

    main.addOption("New Game", &newGame);
    main.addOption("Exit", &quit);

    newGame.addOptionCloseAllMenu("Start", [&tetris](OptionChoice oc) {
        int level = 1;
        try { level = stoi(oc.values["Level"]); } catch (...) {}
        MODE mode = CLASSIC;
        if (oc.values["Lock Down"] == "Extended") mode = EXTENDED;
        else if (oc.values["Lock Down"] == "Infinite") mode = EXTENDED_INFINITY;
        tetris.setStartingLevel(level);
        tetris.setMode(mode);
    });
    newGame.addOptionWithValues("Level", levels);
    newGame.addOptionWithValues("Lock Down", lockDownMode);

    pause.addOptionClose("Resume");
    pause.addOption("Restart", &restartConfirm);
    pause.addOption("Exit Game", &quit);

    restartConfirm.addOptionCloseAllMenu("Yes");
    restartConfirm.addOptionClose("No");

    quit.addOption("Yes", [&tetris](OptionChoice) { tetris.exit(); });
    quit.addOptionClose("No");

    gameOver.addOption("Retry", &newGame, [&]() {
        newGame.setValueChoice("Level", Utility::valueToString(tetris.startingLevel(), 2));
        string modeStr = "Extended";
        if (tetris.mode() == CLASSIC) modeStr = "Classic";
        else if (tetris.mode() == EXTENDED_INFINITY) modeStr = "Infinite";
        newGame.setValueChoice("Lock Down", modeStr);
    });
    gameOver.addOption("Exit Game", &quit);

    main.open();
    tetris.start();

    bool wasTooSmall = false;
    while (!tetris.doExit()) {
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
        tetris.step();
        tetris.render();

        const auto elapsed = chrono::steady_clock::now() - frameStart;
        if (const auto sleepTime = chrono::milliseconds(16) - elapsed;
            sleepTime > chrono::milliseconds(0))
            this_thread::sleep_for(sleepTime);
    }

    SoundEngine::cleanup();
    Input::cleanup();
    Platform::cleanupConsole();

    return 0;
}
