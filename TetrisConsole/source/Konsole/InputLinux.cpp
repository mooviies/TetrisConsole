#include "Input.h"

#include <unistd.h>
#include <sys/select.h>

static bool s_left = false;
static bool s_right = false;
static bool s_hardDrop = false;
static bool s_softDrop = false;
static bool s_rotCW = false;
static bool s_rotCCW = false;
static bool s_hold = false;
static bool s_pause = false;
static bool s_mute = false;
static bool s_select = false;

void Input::init() {
}

void Input::cleanup() {
}

void Input::pollKeys() {
    s_left = false;
    s_right = false;
    s_hardDrop = false;
    s_softDrop = false;
    s_rotCW = false;
    s_rotCCW = false;
    s_hold = false;
    s_pause = false;
    s_mute = false;
    s_select = false;

    unsigned char buf[64];
    fd_set fds;
    timeval tv{};

    while (true) {
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        if (::select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) <= 0)
            break;

        const ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
        if (n <= 0)
            break;

        for (int i = 0; i < n; i++) {
            if (buf[i] == '\033' && i + 2 < n && buf[i + 1] == '[') {
                // ANSI escape sequence
                switch (buf[i + 2]) {
                    case 'A': s_rotCW = true;
                        break; // Up arrow
                    case 'B': s_softDrop = true;
                        break; // Down arrow
                    case 'C': s_right = true;
                        break; // Right arrow
                    case 'D': s_left = true;
                        break; // Left arrow
                    default: ;
                }
                i += 2;
            } else {
                switch (buf[i]) {
                    case ' ': s_hardDrop = true;
                        break;
                    case '\n': // fallthrough
                    case '\r': s_select = true;
                        break;
                    case 0x1B: s_pause = true;
                        break; // Escape (standalone)
                    case 'x':
                    case 'X': s_rotCW = true;
                        break;
                    case 'z':
                    case 'Z': s_rotCCW = true;
                        break;
                    case 'c':
                    case 'C': s_hold = true;
                        break;
                    case 'm':
                    case 'M': s_mute = true;
                        break;
                    default: ;
                }
            }
        }
    }
}

bool Input::left() {
    return s_left;
}

bool Input::right() {
    return s_right;
}

bool Input::hardDrop() {
    return s_hardDrop;
}

bool Input::softDrop() {
    return s_softDrop;
}

bool Input::rotateClockwise() {
    return s_rotCW;
}

bool Input::rotateCounterClockwise() {
    return s_rotCCW;
}

bool Input::hold() {
    return s_hold;
}

bool Input::pause() {
    return s_pause;
}

bool Input::mute() {
    return s_mute;
}

bool Input::select() {
    return s_select;
}

Input::Input() = default;

Input::~Input() = default;
