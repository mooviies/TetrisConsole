#include <chrono>
#include <thread>

#include "Tetrominos.h"
#include "GameMenus.h"
#include "HelpDisplay.h"
#include "HighScoreDisplay.h"
#include "Input.h"
#include "InputSnapshot.h"
#include "SoundEngine.h"
#include "GameRenderer.h"
#include "Platform.h"
#include "rlutil.h"

using namespace std;

static void bindDefaultKeys() {
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
}

static InputSnapshot pollInputSnapshot() {
    auto A = [](Action a) { return static_cast<int>(a); };

    InputSnapshot snapshot;
    snapshot.left = Input::action(A(Action::Left));
    snapshot.right = Input::action(A(Action::Right));
    snapshot.softDrop = Input::action(A(Action::SoftDrop));
    snapshot.hardDrop = Input::action(A(Action::HardDrop));
    snapshot.rotateCW = Input::action(A(Action::RotateCW));
    snapshot.rotateCCW = Input::action(A(Action::RotateCCW));
    snapshot.hold = Input::action(A(Action::Hold));
    snapshot.pause = Input::action(A(Action::Pause));
    return snapshot;
}

int main() {
    Platform::initConsole();
    Input::init(static_cast<int>(Action::Count));
    bindDefaultKeys();

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

    GameMenus menus;
    HighScoreDisplay highScores;
    HelpDisplay help;
    Tetrominos game(menus.pauseMenu(), menus.gameOverMenu(), highScores);
    menus.configure(game, highScores, help);

    while (true) {
        menus.mainMenu().open();
        if (game.doExit()) break;
        game.start();

        bool wasTooSmall = false;
        while (!game.doExit() && !game.backToMenu()) {
            const auto frameStart = chrono::steady_clock::now();

            Input::pollKeys();
            if (Platform::wasResized()) game.redraw();
            if (Platform::isTerminalTooSmall()) {
                if (!wasTooSmall) {
                    game.pauseGameTimer();
                    wasTooSmall = true;
                }
                this_thread::sleep_for(chrono::milliseconds(50));
                continue;
            }
            if (wasTooSmall) {
                game.resumeGameTimer();
                wasTooSmall = false;
            }

            game.step(pollInputSnapshot());
            game.render();

            const auto elapsed = chrono::steady_clock::now() - frameStart;
            if (const auto sleepTime = chrono::milliseconds(16) - elapsed; sleepTime > chrono::milliseconds(0))
                this_thread::sleep_for(sleepTime);
        }
        if (game.doExit()) break;
        game.clearBackToMenu();
        rlutil::cls();
        GameRenderer::renderTitle("A classic in console!");
    }

    SoundEngine::cleanup();
    Input::cleanup();
    Platform::cleanupConsole();

    return 0;
}
