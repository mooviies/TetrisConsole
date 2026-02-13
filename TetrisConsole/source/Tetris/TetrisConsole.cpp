#include <algorithm>
#include <string>

#include "Tetris.h"
#include "Input.h"
#include "SoundEngine.h"
#include "Utility.h"
#include "Menu.h"
#include "Platform.h"
#include "rlutil.h"

using namespace std;

int main() {
    Platform::initConsole();
    Input::init();

    SoundEngine::init();

    while (Platform::isTerminalTooSmall()) {
        rlutil::cls();
        int row = std::max(1, rlutil::trows() / 2);
        rlutil::locate(1, row);
        rlutil::setColor(rlutil::WHITE);
        cout << "  Please resize terminal to 80x29" << flush;
        usleep(100000);
    }

    Utility::showTitle("A classic in console!");

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
    Menu help("HELP");
    Menu quit("Are you sure?");
    Menu gameOver("GAME OVER", "New High Score!");

    Tetris tetris(pause, gameOver);

    Menu::shouldExitGame = [&tetris]() { return tetris.doExit(); };
    Menu::onResize = []() { Utility::showTitle("A classic in console!"); };

    main.addOption("New Game", &newGame);
    //main.addOption("Settings", &options);
    //main.addOption("Help", &help);
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

    //help.addOptionClose("Back");

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
            usleep(50000);
            continue;
        }
        tetris.step();
        SoundEngine::update();
    }

    Input::cleanup();
    Platform::cleanupConsole();

    return 0;
}
