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

    vector<string> modes;
    modes.emplace_back("Extended");
    modes.emplace_back("Infinite");
    modes.emplace_back("Classic");

    Menu main("MAIN MENU");
    Menu options("SETTINGS");
    Menu newGame("NEW GAME");
    Menu pause("PAUSE");
    Menu quit("Are you sure?");
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
        if (oc.values["Mode"] == "Extended") mode = EXTENDED;
        else if (oc.values["Mode"] == "Infinite") mode = EXTENDED_INFINITY;
        tetris.setStartingLevel(level);
        tetris.setMode(mode);
    });
    newGame.addOptionWithValues("Level", levels);
    newGame.addOptionWithValues("Mode", modes);

    options.addOptionWithValues("Level", levels);
    options.addOptionWithValues("Mode", modes);
    options.addOptionCloseAllMenu("Accept", [&tetris](OptionChoice oc) {
        int level = 1;
        try { level = stoi(oc.values["Level"]); } catch (...) {}
        MODE mode = CLASSIC;
        if (oc.values["Mode"] == "Extended") mode = EXTENDED;
        else if (oc.values["Mode"] == "Infinite") mode = EXTENDED_INFINITY;
        tetris.setStartingLevel(level);
        tetris.setMode(mode);
    });
    options.addOptionClose("Cancel");

    pause.addOptionClose("Resume");
    pause.addOptionClose("Restart");
    pause.addOption("Exit Game", &quit);

    quit.addOption("Yes", [&tetris](OptionChoice) { tetris.exit(); });
    quit.addOptionClose("No");

    gameOver.addOptionClose("Retry");
    gameOver.addOption("Settings", &options);
    gameOver.addOption("Exit Game", &quit);

    main.open();
    tetris.start();

    while (!tetris.doExit()) {
        Input::pollKeys();
        if (Platform::wasResized())
            tetris.redraw();
        if (Platform::isTerminalTooSmall()) {
            this_thread::sleep_for(chrono::milliseconds(50));
            continue;
        }
        tetris.step();
        tetris.render();
    }

    SoundEngine::cleanup();
    Input::cleanup();
    Platform::cleanupConsole();

    return 0;
}
