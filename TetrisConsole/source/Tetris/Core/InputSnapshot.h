#pragma once

enum class Action { Left, Right, SoftDrop, HardDrop, RotateCW, RotateCCW, Hold, Pause, Mute, Select, Count };

struct InputSnapshot {
    bool left{}, right{}, softDrop{}, hardDrop{};
    bool rotateCW{}, rotateCCW{};
    bool hold{}, pause{}, mute{};
};
