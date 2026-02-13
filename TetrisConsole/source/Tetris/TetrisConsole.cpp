#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <queue>

#include "Tetris.h"
#include "Random.h"
#include "Input.h"
#include "SoundEngine.h"
#include "Utility.h"
#include "Menu.h"
#include "Overseer.h"
#include "Platform.h"

using namespace std;

void setGameSettings(OptionChoice optionChoice);
void quitGame(OptionChoice optionChoice);
void incrementLevel(OptionChoice optionChoice);
void decrementLevel(OptionChoice optionChoice);

int main()
{
	Platform::initConsole();
	Input::init();

	Random::init();
	SoundEngine::init();

	Utility::afficherTitre("A classic in console!");

	vector<string> levels;
	for (int i = 1; i <= 15; i++)
		levels.push_back(Utility::valueToString(i, 2));

	vector<string> modes;
	modes.push_back("Extended");
	modes.push_back("Infinite");
	modes.push_back("Classic");

	Menu main("MAIN MENU");
	Menu options("SETTINGS");
	Menu newGame("NEW GAME");
	Menu pause("PAUSE");
	Menu help("HELP");
	Menu quit("Are you sure?");
	Menu gameover("GAME OVER", "New High Score!");

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

	gameover.addOptionClose("Retry");
	gameover.addOption("Settings", &options);
	gameover.addOption("Exit Game", &quit);

	Tetris tetris(pause, gameover);
	Overseer::setTetris(&tetris);

	main.open();
	tetris.start();

	while (!tetris.doExit())
	{
		Input::pollKeys();
		tetris.step();
		SoundEngine::update();
	}

	Input::cleanup();
	Platform::cleanupConsole();

    return 0;
}

void setGameSettings(OptionChoice optionChoice)
{
	string levelName = optionChoice.values["Level"];
	string modeName = optionChoice.values["Mode"];

	int level = stoi(levelName);
	MODE mode;
	if (modeName == "Extended")
	{
		mode = EXTENDED;
	}
	else if (modeName == "Infinite")
	{
		mode = EXTENDED_INFINITY;
	}
	else if (modeName == "Classic")
	{
		mode = CLASSIC;
	}

	Overseer::getTetris().setStartingLevel(level);
	Overseer::getTetris().setMode(mode);
}

void quitGame(OptionChoice optionChoice)
{
	Input::cleanup();
	Platform::cleanupConsole();
	exit(0);
}

void incrementLevel(OptionChoice optionChoice)
{

}

void decrementLevel(OptionChoice optionChoice)
{

}
