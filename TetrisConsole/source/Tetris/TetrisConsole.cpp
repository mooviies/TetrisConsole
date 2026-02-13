#include <string>

#include "Tetris.h"
#include "Input.h"
#include "SoundEngine.h"
#include "Utility.h"
#include "Menu.h"
#include "Overseer.h"
#include "Platform.h"

using namespace std;

void setGameSettings(OptionChoice optionChoice);
void quitGame(OptionChoice optionChoice);

int main() {
    Platform::initConsole();
    Input::init();

    SoundEngine::init();

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

    main.addOption("New Game", &newGame);
    //main.addOption("Settings", &options);
    //main.addOption("Help", &help);
    main.addOption("Exit", &quit);

    newGame.addOptionCloseAllMenu("Start", &setGameSettings);
    newGame.addOptionWithValues("Level", levels);
    newGame.addOptionWithValues("Mode", modes);

    //help.addOptionClose("Back");

    options.addOptionWithValues("Level", levels);
    options.addOptionWithValues("Mode", modes);
    options.addOptionCloseAllMenu("Accept", &setGameSettings);
    options.addOptionClose("Cancel");

    pause.addOptionClose("Resume");
    pause.addOptionClose("Restart");
    pause.addOption("Exit Game", &quit);

    quit.addOption("Yes", &quitGame);
    quit.addOptionClose("No");

    gameOver.addOptionClose("Retry");
    gameOver.addOption("Settings", &options);
    gameOver.addOption("Exit Game", &quit);

    Tetris tetris(pause, gameOver);
    Overseer::setTetris(&tetris);

    main.open();
    tetris.start();

    while (!tetris.doExit()) {
        Input::pollKeys();
        tetris.step();
        SoundEngine::update();
    }

    Input::cleanup();
    Platform::cleanupConsole();

    return 0;
}

void setGameSettings(OptionChoice optionChoice) {
    const string levelName = optionChoice.values["Level"];
    const string modeName = optionChoice.values["Mode"];

    int level = 1;
    try { level = stoi(levelName); } catch (...) {}
    MODE mode = CLASSIC;
    if (modeName == "Extended") {
        mode = EXTENDED;
    } else if (modeName == "Infinite") {
        mode = EXTENDED_INFINITY;
    } else if (modeName == "Classic") {
        mode = CLASSIC;
    }

    Overseer::getTetris().setStartingLevel(level);
    Overseer::getTetris().setMode(mode);
}

void quitGame([[maybe_unused]] OptionChoice optionChoice) {
    Overseer::getTetris().exit();
}
