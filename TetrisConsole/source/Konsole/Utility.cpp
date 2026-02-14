#include "Utility.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Panel.h"
#include "Platform.h"

using namespace std;

void Utility::showTitle(const string& subtitle)
{
	int ox = Platform::offsetX();
	int oy = Platform::offsetY();

	Panel title(78);
	title.addRow("Tetris Console", Align::CENTER);
	title.addSeparator();
	title.addRow(subtitle, Align::CENTER);
	title.setPosition(1 + ox, 1 + oy);
	title.render();
}

string Utility::valueToString(int64_t value, int length)
{
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(length) << value;
	return oss.str();
}
