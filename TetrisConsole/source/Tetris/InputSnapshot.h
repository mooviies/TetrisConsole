#pragma once

struct InputSnapshot {
	bool left{}, right{}, softDrop{}, hardDrop{};
	bool rotateCW{}, rotateCCW{};
	bool hold{}, pause{}, mute{};
};
