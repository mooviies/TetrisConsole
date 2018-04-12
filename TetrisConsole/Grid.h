#pragma once

class Grid
{
public:
	Grid();
	Grid(int width);
	~Grid();

	int getWidth() const;
	int getValue(int x, int y) const;
	void setValue(int x, int y, int value);

private:
	int** _grid;
	int _width;
};

