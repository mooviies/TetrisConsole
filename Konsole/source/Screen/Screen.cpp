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
	setColor(_currentBackgroundColor, true);
	
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

void Screen::draw(uint x, uint y, char c)
{
	if (!_initialized)
		return;

	setCursorPosition(x, y);
	cout << c;
}

void Screen::draw(uint x, uint y, char c, Color color, Color backgroundColor)
{
	if (!_initialized)
		return;

	Color previousColor = _currentColor, previousBackgroundColor = _currentBackgroundColor;
	setColor(color);
	setBackgroundColor(backgroundColor);
	draw(x, y, c);
	setColor(previousColor);
	setBackgroundColor(previousBackgroundColor);
}

void Screen::draw(uint x, uint y, const char* str)
{
	if (!_initialized)
		return;

	setCursorPosition(x, y);
	cout << str;
}

void Screen::draw(uint x, uint y, const char* str, Color color, Color backgroundColor)
{
	if (!_initialized)
		return;

	Color previousColor = _currentColor, previousBackgroundColor = _currentBackgroundColor;
	setColor(color);
	setBackgroundColor(backgroundColor);
	draw(x, y, str);
	setColor(previousColor);
	setBackgroundColor(previousBackgroundColor);
}

void Screen::draw(uint x, uint y, const std::string& str)
{
	if (!_initialized)
		return;

	setCursorPosition(x, y);
	cout << str;
}

void Screen::draw(uint x, uint y, const std::string& str, Color color, Color backgroundColor)
{
	if (!_initialized)
		return;

	Color previousColor = _currentColor, previousBackgroundColor = _currentBackgroundColor;
	setColor(color);
	setBackgroundColor(backgroundColor);
	draw(x, y, str);
	setColor(previousColor);
	setBackgroundColor(previousBackgroundColor);
}

void Screen::draw(uint x, uint y, uint length)
{
	if (!_initialized)
		return;

	setCursorPosition(x, y);
	cout << string(length, ' ');
}

void Screen::draw(uint x, uint y, Color color, uint length)
{
	if (!_initialized)
		return;

	if (length == 0)
		length = 1;

	Color previousBackgroundColor = _currentBackgroundColor;
	setBackgroundColor(color);
	draw(x, y, length);
	setBackgroundColor(previousBackgroundColor);
}

void Screen::setSize(uint width, uint height)
{
	if (width == 0) width = 1;
	if (height == 0) height = 1;

	_width = width;
	_height = height;

	onSetSize(width, height);
}

void Screen::setColor(Color color, bool force)
{
	if (_currentColor == color)
		return;

	_currentColor = color;
	onSetColor(color);
}

void Screen::setBackgroundColor(Color color, bool force)
{
	if (_currentBackgroundColor == color)
		return;

	_currentBackgroundColor = color;
	onSetBackgroundColor(color);
}

void Screen::setCursorPosition(uint x, uint y)
{
	onSetCursorPosition(x, y);
}