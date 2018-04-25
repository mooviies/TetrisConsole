#include "Screen.h"

#include <iostream>

using namespace konsole;
using namespace std;

Screen::Screen(uint width, uint height, Color initialColor, Color initialBackgroundColor)
	: _initialized(false), _width(width), _height(height), _currentColor(initialColor), _currentBackgroundColor(initialBackgroundColor)
{
}

Screen::~Screen()
{
	
}

void Screen::initialize()
{
	onInitialize();

	setSize(_width, _height);
	setColor(_currentColor, true);
	setBackgroundColor(_currentBackgroundColor, true);
	
	_initialized = true;
}

bool Screen::isInitialized() const
{
	return _initialized;
}

uint Screen::getWidth() const
{
	return _width;
}

uint Screen::getHeight() const
{
	return _height;
}

Color Screen::getColor() const
{
	return _currentColor;
}

Color Screen::getBackgroundColor() const
{
	return _currentBackgroundColor;
}

void Screen::setTitle(const std::wstring& title)
{
	onSetTitle(title);
}

void Screen::setColor(Color color)
{
	if (!_initialized)
		return;

	setColor(color, false);
}

void Screen::setBackgroundColor(Color color)
{
	if (!_initialized)
		return;

	setBackgroundColor(color, false);
}

void Screen::draw(uint x, uint y, const GraphicObject* obj, Alignement alignement)
{
	size_t height = obj->getHeight();
	for (size_t i = 0; i < height; i++)
	{
		setCursorPosition(x, y + i);
		draw(x, y + i, obj->getLine(i, alignement));
	}

	size_t nbChild = obj->getNbChild();
	for (size_t i = 0; i < nbChild; i++)
	{
		const GraphicObject* child = obj->getChild(i);
		Coordinates coord = child->getRelativeCoordinates(i);
		draw(x + coord.x, y + coord.y, child, alignement);
	}
}

void Screen::draw(uint x, uint y, const ColoredString& str)
{
	for (int i = 0; i < str.length(); i++)
	{
		draw(x, y, str[i]);
	}
}

void Screen::draw(uint x, uint y, const ColoredChar& c)
{
	if (!_initialized)
		return;

	if (!isValidPosition(x, y))
		return;

	setColor(c.textColor);
	setBackgroundColor(c.backgroundColor);
	cout << c.value;
}

void Screen::draw(uint x, uint y, Color color, uint length)
{
	if (!_initialized)
		return;

	if (length == 0)
		length = 1;

	draw(x, y, string(length, ' '));
}

void Screen::setSize(uint width, uint height)
{
	if (width == 0) width = 1;
	if (height == 0) height = 1;

	_width = width;
	_height = height;

	onSetSize(_width, _height);
}

void Screen::setColor(Color color, bool force)
{
	if (_currentColor == color)
		return;

	_currentColor = color;
	onSetColor(_currentColor);
}

void Screen::setBackgroundColor(Color color, bool force)
{
	if (_currentBackgroundColor == color)
		return;

	_currentBackgroundColor = color;
	onSetBackgroundColor(_currentBackgroundColor);
}

void Screen::setCursorPosition(uint x, uint y)
{
	onSetCursorPosition(x, y);
}

bool Screen::isValidPosition(uint x, uint y) const
{
	return (x < _width) && (y < _height);
}