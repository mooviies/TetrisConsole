#include "GameEngine.h"
#include "Platform.h"

#include <chrono>
#include <thread>

using namespace std;

int GameEngine::run() {
    Platform::initConsole();

    while (Platform::isTerminalTooSmall()) {
        Platform::showResizePrompt();
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    onInit();

    bool wasTooSmall = false;
    auto previousFrame = chrono::steady_clock::now();

    while (!_exitRequested) {
        const auto frameStart = chrono::steady_clock::now();
        const double dt = chrono::duration<double>(frameStart - previousFrame).count();
        previousFrame = frameStart;

        if (Platform::wasResized())
            onResize();

        if (Platform::isTerminalTooSmall()) {
            if (!wasTooSmall) {
                onTerminalTooSmall();
                wasTooSmall = true;
            }
            this_thread::sleep_for(chrono::milliseconds(50));
            continue;
        }

        if (wasTooSmall) {
            onTerminalRestored();
            wasTooSmall = false;
        }

        onFrame(dt);

        const auto elapsed = chrono::steady_clock::now() - frameStart;
        const auto budget = chrono::milliseconds(1000 / _targetFps);
        if (const auto sleepTime = budget - elapsed; sleepTime > chrono::milliseconds(0))
            this_thread::sleep_for(sleepTime);
    }

    onCleanup();
    Platform::cleanupConsole();

    return _exitCode;
}

void GameEngine::requestExit(int exitCode) {
    _exitRequested = true;
    _exitCode = exitCode;
}

void GameEngine::setTargetFps(int fps) {
    _targetFps = fps;
}

bool GameEngine::exitRequested() const {
    return _exitRequested;
}
