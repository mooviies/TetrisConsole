#include "TetrominosGame.h"

#include "GameMenus.h"
#include "GameRenderer.h"
#include "HelpDisplay.h"
#include "HighScoreDisplay.h"
#include "Input.h"
#include "InputSnapshot.h"
#include "SoundEngine.h"
#include "Tetrominos.h"
#include "rlutil.h"

using namespace std;

TetrominosGame::TetrominosGame() = default;
TetrominosGame::~TetrominosGame() = default;

void TetrominosGame::bindDefaultKeys() {
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

InputSnapshot TetrominosGame::pollInputSnapshot() {
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

void TetrominosGame::onInit() {
    Input::init(static_cast<int>(Action::Count));
    bindDefaultKeys();

    if (!SoundEngine::init()) {
        Input::cleanup();
        requestExit(1);
        return;
    }

    GameRenderer::renderTitle("A classic in console!");

    _menus = make_unique<GameMenus>();
    _highScores = make_unique<HighScoreDisplay>();
    _help = make_unique<HelpDisplay>();
    _game = make_unique<Tetrominos>(_menus->pauseMenu(), _menus->gameOverMenu(), *_highScores);
    _menus->configure(*_game, *_highScores, *_help);

    _screen = Screen::MainMenu;
}

void TetrominosGame::onFrame(double /*dt*/) {
    switch (_screen) {
    case Screen::MainMenu:
        _menus->mainMenu().open();
        if (_game->doExit()) {
            requestExit();
            return;
        }
        _game->start();
        _screen = Screen::Playing;
        break;

    case Screen::Playing:
        Input::pollKeys();
        _game->step(pollInputSnapshot());
        _game->render();

        if (_game->doExit()) {
            requestExit();
            return;
        }
        if (_game->backToMenu()) {
            _game->clearBackToMenu();
            rlutil::cls();
            GameRenderer::renderTitle("A classic in console!");
            _screen = Screen::MainMenu;
        }
        break;
    }
}

void TetrominosGame::onCleanup() {
    _game.reset();
    _help.reset();
    _highScores.reset();
    _menus.reset();
    SoundEngine::cleanup();
    Input::cleanup();
}

void TetrominosGame::onResize() {
    if (_screen == Screen::Playing)
        _game->redraw();
}

void TetrominosGame::onTerminalTooSmall() {
    if (_screen == Screen::Playing)
        _game->pauseGameTimer();
}

void TetrominosGame::onTerminalRestored() {
    if (_screen == Screen::Playing)
        _game->resumeGameTimer();
}
