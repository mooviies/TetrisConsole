#pragma once

class GameEngine {
public:
    virtual ~GameEngine() = default;
    int run();
    void requestExit(int exitCode = 0);

protected:
    virtual void onInit() = 0;
    virtual void onFrame(double dt) = 0;
    virtual void onCleanup() {}
    virtual void onResize() {}
    virtual void onTerminalTooSmall() {}
    virtual void onTerminalRestored() {}

    void setTargetFps(int fps);
    [[nodiscard]] bool exitRequested() const;

private:
    bool _exitRequested{};
    int _exitCode{};
    int _targetFps{60};
};
