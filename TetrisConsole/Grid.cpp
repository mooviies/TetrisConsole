#include "stdafx.h"
#include "Grid.h"


Grid::Grid()
{
	_width = 0;
	_grid = NULL;
}

Grid::Grid(int width)
	: _width(width)
{
	_grid = new int*[_width];
	for (int i = 0; i < _width; i++)
		_grid[i] = new  int[width];
}


Grid::~Grid()
{
	for (int i = 0; i < _width; i++)
		delete _grid[i];

	if(_grid != NULL)
		delete _grid;
}

int Grid::getWidth() const
{
	return _width;
}

int Grid::getValue(int x, int y) const
{
	if(x >= 0 && x < _width && y >= 0 && y < _width)
		return _grid[x][y];
	
	return 0;
}

void Grid::setValue(int x, int y, int value)
{
	if(x >= 0 && x < _width && y >= 0 && y < _width)
		_grid[x][y] = value;
}