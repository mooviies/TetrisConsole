#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "Tetris.h"
#include "Input.h"
#include "SoundEngine.h"
#include "Utility.h"
#include "GameRenderer.h"
#include "Menu.h"
#include "Platform.h"
#include "rlutil.h"

using namespace std;

static void showHighScores(const Tetris& tetris,
                           const vector<string>& levels,
                           const vector<string>& lockDownModes) {
    constexpr int kInterior = 24;
    constexpr int kLabelWidth = 12;

    Panel panel(kInterior);
    panel.addRow("HIGH SCORES", Align::CENTER);
    panel.addSeparator();
    size_t levelRow = panel.addRow("", Align::LEFT);
    size_t modeRow  = panel.addRow("", Align::LEFT);
    panel.addSeparator();
    size_t scoreRow = panel.addRow("", Align::CENTER);
    size_t timeRow  = panel.addRow("", Align::CENTER);
    panel.addSeparator();
    size_t lvlStatRow    = panel.addRow({Cell("Level", Align::LEFT, 15, kLabelWidth),
                                         Cell("", Align::CENTER)});
    size_t tpmStatRow    = panel.addRow({Cell("TPM", Align::LEFT, 15, kLabelWidth),
                                         Cell("", Align::CENTER)});
    size_t lpmStatRow    = panel.addRow({Cell("LPM", Align::LEFT, 15, kLabelWidth),
                                         Cell("", Align::CENTER)});
    size_t linesStatRow  = panel.addRow({Cell("Lines", Align::LEFT, 15, kLabelWidth),
                                         Cell("", Align::CENTER)});
    size_t tetrisStatRow = panel.addRow({Cell("Tetris", Align::LEFT, 15, kLabelWidth),
                                         Cell("", Align::CENTER)});
    size_t combosStatRow = panel.addRow({Cell("Combos", Align::LEFT, 15, kLabelWidth),
                                         Cell("", Align::CENTER)});
    size_t tSpinsStatRow = panel.addRow({Cell("T-Spins", Align::LEFT, 15, kLabelWidth),
                                         Cell("", Align::CENTER)});

    constexpr int windowWidth = 80;
    constexpr int windowHeight = 28;
    int w = panel.width();
    int h = panel.height();
    int px = Platform::offsetX() + (windowWidth / 2) - (w / 2);
    int py = Platform::offsetY() + (windowHeight / 2) - (h / 2);
    panel.setPosition(px, py);

    int choice = 0;
    int levelIdx = 0;
    int modeIdx = 0;

    const auto& hsMap = tetris.highscoreMap();

    auto updateDisplay = [&]() {
        string lp = (choice == 0) ? "> " : "  ";
        string mp = (choice == 1) ? "> " : "  ";
        panel.setCell(levelRow, 0,
                      lp + "Level     : " + levels[static_cast<size_t>(levelIdx)]);
        panel.setCell(modeRow, 0,
                      mp + "Lock Down : " + lockDownModes[static_cast<size_t>(modeIdx)]);

        int lvl = levelIdx + 1;
        MODE mode = EXTENDED;
        if (modeIdx == 1) mode = EXTENDED_INFINITY;
        else if (modeIdx == 2) mode = CLASSIC;

        HighScoreKey key{lvl, mode};
        auto it = hsMap.find(key);

        if (it != hsMap.end()) {
            const auto& rec = it->second;
            panel.setCell(scoreRow, 0, Utility::valueToString(rec.score, 10));
            panel.setCell(timeRow, 0, Utility::timeToString(rec.gameElapsed));
            panel.setCell(lvlStatRow, 1, Utility::valueToString(rec.level, 2));
            panel.setCell(tpmStatRow, 1, Utility::valueToString(rec.tpm, 6));
            panel.setCell(lpmStatRow, 1, Utility::valueToString(rec.lpm, 6));
            panel.setCell(linesStatRow, 1, Utility::valueToString(rec.lines, 6));
            panel.setCell(tetrisStatRow, 1, Utility::valueToString(rec.tetris, 6));
            panel.setCell(combosStatRow, 1, Utility::valueToString(rec.combos, 6));
            panel.setCell(tSpinsStatRow, 1, Utility::valueToString(rec.tSpins, 6));
        } else {
            panel.setCell(scoreRow, 0, "----------");
            panel.setCell(timeRow, 0, "--:--.--");
            panel.setCell(lvlStatRow, 1, "--");
            panel.setCell(tpmStatRow, 1, "------");
            panel.setCell(lpmStatRow, 1, "------");
            panel.setCell(linesStatRow, 1, "------");
            panel.setCell(tetrisStatRow, 1, "------");
            panel.setCell(combosStatRow, 1, "------");
            panel.setCell(tSpinsStatRow, 1, "------");
        }
    };

    updateDisplay();
    Platform::flushInput();

    while (true) {
        if (!Platform::isTerminalTooSmall()) {
            panel.render();
            cout << flush;
        }

        switch (Platform::getKey()) {
            case rlutil::KEY_UP:
            case rlutil::KEY_DOWN:
                choice = (choice == 0) ? 1 : 0;
                updateDisplay();
                break;
            case rlutil::KEY_LEFT:
                if (choice == 0) {
                    levelIdx--;
                    if (levelIdx < 0)
                        levelIdx = static_cast<int>(levels.size()) - 1;
                } else {
                    modeIdx--;
                    if (modeIdx < 0)
                        modeIdx = static_cast<int>(lockDownModes.size()) - 1;
                }
                updateDisplay();
                break;
            case rlutil::KEY_RIGHT:
                if (choice == 0) {
                    levelIdx++;
                    if (levelIdx >= static_cast<int>(levels.size()))
                        levelIdx = 0;
                } else {
                    modeIdx++;
                    if (modeIdx >= static_cast<int>(lockDownModes.size()))
                        modeIdx = 0;
                }
                updateDisplay();
                break;
            case rlutil::KEY_ESCAPE:
            case rlutil::KEY_ENTER:
                panel.clear();
                return;
            default:
                break;
        }

        if (Platform::wasResized()) {
            if (!Platform::isTerminalTooSmall()) {
                if (Menu::onResize)
                    Menu::onResize();
                px = Platform::offsetX() + (windowWidth / 2) - (w / 2);
                py = Platform::offsetY() + (windowHeight / 2) - (h / 2);
                panel.setPosition(px, py);
                panel.invalidate();
            }
            continue;
        }

        if (Menu::shouldExitGame && Menu::shouldExitGame())
            break;
    }

    panel.clear();
}

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
    Menu backToMenuConfirm("Back to menu?");
    Menu quit("Quit game?");
    Menu gameOver("GAME OVER", "New High Score!");

    Tetris tetris(pause, gameOver);

    Menu::shouldExitGame = [&tetris]() { return tetris.doExit(); };
    Menu::onResize = []() { GameRenderer::renderTitle("A classic in console!"); };

    main.addOption("New Game", &newGame);
    main.addOptionAction("High Scores", [&]() { showHighScores(tetris, levels, lockDownMode); });
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
    pause.addOption("Main Menu", &backToMenuConfirm);
    pause.addOption("Exit Game", &quit);

    restartConfirm.addOptionCloseAllMenu("Yes");
    restartConfirm.addOptionClose("No");

    backToMenuConfirm.addOptionCloseAllMenu("Yes");
    backToMenuConfirm.addOptionClose("No");

    quit.addOption("Yes", [&tetris](OptionChoice) { tetris.exit(); });
    quit.addOptionClose("No");

    gameOver.addOption("Retry", &newGame, [&]() {
        newGame.setValueChoice("Level", Utility::valueToString(tetris.startingLevel(), 2));
        string modeStr = "Extended";
        if (tetris.mode() == CLASSIC) modeStr = "Classic";
        else if (tetris.mode() == EXTENDED_INFINITY) modeStr = "Infinite";
        newGame.setValueChoice("Lock Down", modeStr);
    });
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
            tetris.step();
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
