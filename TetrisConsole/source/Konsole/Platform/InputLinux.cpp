#include "Input.h"

#include <unistd.h>
#include <sys/select.h>
#include <algorithm>
#include <vector>
#include <cctype>

void Input::pollKeys() {
    std::fill(s_actions.begin(), s_actions.end(), false);

    std::vector<KeyCode> pressed;
    unsigned char buf[64];
    fd_set fds;
    timeval tv{};

    while (true) {
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        if (::select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) <= 0) break;

        const ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
        if (n <= 0) break;

        for (ssize_t i = 0; i < n; i++) {
            if (buf[i] == '\033') {
                if (i + 2 < n && buf[i + 1] == '[') {
                    unsigned char c = buf[i + 2];
                    // Arrow keys: ESC [ A/B/C/D
                    if (c == 'A') {
                        pressed.push_back(KeyCode::ArrowUp);
                        i += 2;
                    } else if (c == 'B') {
                        pressed.push_back(KeyCode::ArrowDown);
                        i += 2;
                    } else if (c == 'C') {
                        pressed.push_back(KeyCode::ArrowRight);
                        i += 2;
                    } else if (c == 'D') {
                        pressed.push_back(KeyCode::ArrowLeft);
                        i += 2;
                    } else if (c >= '0' && c <= '9') {
                        // ESC [ number ~ sequences
                        int num = c - '0';
                        i += 2;
                        // Consume additional digits
                        while (i + 1 < n && buf[i + 1] >= '0' && buf[i + 1] <= '9') {
                            i++;
                            num = num * 10 + (buf[i] - '0');
                        }
                        if (i + 1 < n && buf[i + 1] == '~') {
                            i++;
                            switch (num) {
                                case 1: pressed.push_back(KeyCode::Home); break;
                                case 2: pressed.push_back(KeyCode::Insert); break;
                                case 3: pressed.push_back(KeyCode::Delete); break;
                                case 4: pressed.push_back(KeyCode::End); break;
                                case 5: pressed.push_back(KeyCode::PageUp); break;
                                case 6: pressed.push_back(KeyCode::PageDown); break;
                                case 15: pressed.push_back(KeyCode::F5); break;
                                case 17: pressed.push_back(KeyCode::F6); break;
                                case 18: pressed.push_back(KeyCode::F7); break;
                                case 19: pressed.push_back(KeyCode::F8); break;
                                case 20: pressed.push_back(KeyCode::F9); break;
                                case 21: pressed.push_back(KeyCode::F10); break;
                                case 23: pressed.push_back(KeyCode::F11); break;
                                case 24: pressed.push_back(KeyCode::F12); break;
                                default: break;
                            }
                        }
                    } else {
                        i += 2; // skip unknown ESC [ X
                    }
                } else if (i + 2 < n && buf[i + 1] == 'O') {
                    // ESC O P/Q/R/S → F1–F4
                    switch (buf[i + 2]) {
                        case 'P': pressed.push_back(KeyCode::F1); break;
                        case 'Q': pressed.push_back(KeyCode::F2); break;
                        case 'R': pressed.push_back(KeyCode::F3); break;
                        case 'S': pressed.push_back(KeyCode::F4); break;
                        default: break;
                    }
                    i += 2;
                } else {
                    // Standalone ESC
                    pressed.push_back(KeyCode::Escape);
                }
            } else {
                unsigned char c = buf[i];
                if (c == '\n' || c == '\r')
                    pressed.push_back(KeyCode::Enter);
                else if (c == '\t')
                    pressed.push_back(KeyCode::Tab);
                else if (c == 0x7F)
                    pressed.push_back(KeyCode::Backspace);
                else if (c == ' ')
                    pressed.push_back(KeyCode(static_cast<int>(' ')));
                else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                    pressed.push_back(KeyCode(static_cast<int>(std::toupper(c))));
                else if (c >= '0' && c <= '9') {
                    pressed.push_back(KeyCode(static_cast<int>(c)));
                    // Numpad digits are indistinguishable from regular digits
                    // in a terminal, so also emit the Numpad keycode
                    pressed.push_back(KeyCode(static_cast<int>(KeyCode::Numpad0) + (c - '0')));
                } else if (c >= 0x01 && c <= 0x1A) {
                    // Ctrl+letter produces bytes 0x01–0x1A
                    pressed.push_back(KeyCode::Control);
                    pressed.push_back(KeyCode(static_cast<int>('A') + c - 0x01));
                }
            }
        }
    }

    // Map pressed keys to actions
    for (size_t a = 0; a < s_bindings.size(); a++) {
        s_actions[a] = std::any_of(s_bindings[a].begin(), s_bindings[a].end(), [&pressed](KeyCode key) {
            return std::any_of(pressed.begin(), pressed.end(), [key](KeyCode p) { return p == key; });
        });
    }
}
