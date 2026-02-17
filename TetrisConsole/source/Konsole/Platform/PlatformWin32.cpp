#include "Platform.h"

#include <windows.h>
#include <mmsystem.h>
#include <shlobj.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <streambuf>

#include "rlutil.h"

// ---------------------------------------------------------------------------
// Batching streambuf: collects all std::cout output in memory, then writes
// the entire buffer to the console in a single WriteConsoleA call when
// sync() is triggered (by std::flush).  This avoids hundreds of individual
// round-trips through Windows Terminal's ConPTY layer per frame.
// ---------------------------------------------------------------------------
class BatchingStreambuf : public std::streambuf {
public:
    explicit BatchingStreambuf(std::streambuf *target) : _target(target) { _buf.reserve(65536); }

    void install() { _prev = std::cout.rdbuf(this); }
    void uninstall() {
        flush();
        if (_prev) std::cout.rdbuf(_prev);
    }

    void flush() {
        if (!_buf.empty()) {
            HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD written;
            WriteFile(out, _buf.data(), static_cast<DWORD>(_buf.size()), &written, nullptr);
            _buf.clear();
        }
    }

protected:
    int_type overflow(int_type c) override {
        if (c != EOF) _buf += static_cast<char>(c);
        return c;
    }

    std::streamsize xsputn(const char *s, std::streamsize n) override {
        _buf.append(s, static_cast<size_t>(n));
        return n;
    }

    int sync() override {
        flush();
        return 0;
    }

private:
    std::streambuf *_target;
    std::streambuf *_prev = nullptr;
    std::string _buf;
};

static BatchingStreambuf *s_batcher = nullptr;

void Platform::initConsole() {
    // Set timer resolution to 1ms so Sleep() is precise (default ~15.6ms
    // granularity halves the frame rate and slows gravity at higher levels).
    timeBeginPeriod(1);

    SetConsoleOutputCP(CP_UTF8);

    // Ignore Ctrl+C — quit through the in-app menu
    SetConsoleCtrlHandler(NULL, TRUE);

    HWND console = GetConsoleWindow();
    SetWindowLongPtr(console, GWL_STYLE, GetWindowLongPtr(console, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX));
    SetConsoleTitle(TEXT("Tetris Console"));

    system("color 0F");

    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    // Enable VT processing so escape sequences work (needed for Windows Terminal)
    DWORD outMode = 0;
    GetConsoleMode(out, &outMode);
    SetConsoleMode(out, outMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);

    // Force exactly 80x29 character cells.
    // Console APIs work on legacy conhost; the escape sequence works on
    // Windows Terminal (Win 11 default) where the console APIs are ignored.
    SMALL_RECT minWindow = {0, 0, 0, 0};
    SetConsoleWindowInfo(out, TRUE, &minWindow);
    COORD bufferSize = {80, 29};
    SetConsoleScreenBufferSize(out, bufferSize);
    SMALL_RECT windowSize = {0, 0, 79, 28};
    SetConsoleWindowInfo(out, TRUE, &windowSize);

    // xterm resize sequence — Windows Terminal respects this
    std::cout << "\033[8;29;80t" << std::flush;

    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prev_mode;
    GetConsoleMode(input, &prev_mode);
    SetConsoleMode(input, prev_mode & ~ENABLE_QUICK_EDIT_MODE);

    // Install batching streambuf AFTER all init output is flushed.
    // All subsequent std::cout writes are collected in memory and sent
    // to the console in one shot per flush (see flushOutput).
    static BatchingStreambuf batcher(std::cout.rdbuf());
    s_batcher = &batcher;
    s_batcher->install();
}

void Platform::cleanupConsole() {
    if (s_batcher) {
        s_batcher->uninstall();
        s_batcher = nullptr;
    }
    timeEndPeriod(1);
}

void Platform::flushInput() {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

void Platform::flushOutput() {
    std::cout << std::flush;
}

int Platform::getKey() {
    // Poll so we can detect resize between keypresses, similar to how
    // Linux's read() is interrupted by SIGWINCH.  Return -1 on resize
    // so the caller's switch falls through to its wasResized() check.
    while (true) {
        if (_kbhit()) return rlutil::getkey();
        if (rlutil::tcols() != 80 || rlutil::trows() != 29) return -1;
        Sleep(10);
    }
}

int Platform::getKeyTimeout(int timeoutMs) {
    for (int elapsed = 0; elapsed < timeoutMs; elapsed++) {
        if (_kbhit()) return getKey();
        Sleep(1);
    }
    return -1;
}

bool Platform::wasResized() {
    if (rlutil::tcols() != 80 || rlutil::trows() != 29) {
        // Flush pending output, send resize, wait, then clear
        std::cout << "\033[8;29;80t" << std::flush;
        Sleep(50);
        rlutil::cls();
        std::cout << std::flush;
        return true;
    }
    return false;
}

bool Platform::isTerminalTooSmall() {
    return rlutil::tcols() < 80 || rlutil::trows() < 29;
}

void Platform::showResizePrompt() {
}

int Platform::offsetX() {
    return 0;
}

int Platform::offsetY() {
    return 0;
}

void Platform::updateOffsets() {
}

std::string Platform::getDataDir() {
    char path[MAX_PATH];
    std::string dir;
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path)))
        dir = std::string(path) + "\\TetrisConsole";
    else
        dir = ".\\TetrisConsole";
    CreateDirectoryA(dir.c_str(), NULL);
    return dir;
}
