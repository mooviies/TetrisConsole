#pragma once

#include <array>

#include "InputSnapshot.h"
#include "Panel.h"

class HelpDisplay {
public:
    HelpDisplay();

    void open();

    static constexpr int kMaxKeyCols = 3;

private:
    void reposition();
    void refreshBindings();

    Panel _leftPanel;
    Panel _rightPanel;

    static constexpr int kControlCount = 9;
    static constexpr int kActions[kControlCount] = {
        static_cast<int>(Action::Left),     static_cast<int>(Action::Right),    static_cast<int>(Action::SoftDrop),
        static_cast<int>(Action::HardDrop), static_cast<int>(Action::RotateCW), static_cast<int>(Action::RotateCCW),
        static_cast<int>(Action::Hold),     static_cast<int>(Action::Pause),    static_cast<int>(Action::Mute),
    };
    std::array<size_t, kControlCount> _controlRows{};
};
