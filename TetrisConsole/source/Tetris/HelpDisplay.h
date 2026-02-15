#pragma once

#include "Panel.h"

class HelpDisplay {
public:
	HelpDisplay();

	void open();

private:
	void reposition();

	Panel _leftPanel;
	Panel _rightPanel;
};
