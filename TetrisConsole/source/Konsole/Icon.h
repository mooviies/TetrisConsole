#pragma once

#include <string>

class Icon {
public:
	explicit Icon(std::string symbol, int color = 15);

	void setPosition(int x, int y);
	void setColor(int color);
	void draw() const;

private:
	std::string _symbol;
	int _color;
	int _x = 0;
	int _y = 0;
};
