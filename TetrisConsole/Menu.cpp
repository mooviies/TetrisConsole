#include "stdafx.h"
#include "Menu.h"

#include <Windows.h>
#include "rlutil.h"

#define MINIMUM_INTERIOR_WIDTH 13

Menu::Menu(string name)
	: _name(name), _choice(0), _longestOptionWithChoice(0), _longestOption(0), _longestOptionValue(0), _x(0), _y(0), _width(0), _height(0), _close(false)
{
}

Menu::~Menu()
{
}

void Menu::addOption(string name, Menu *menu)
{
	addOption(name);
	_menus[name] = menu;
}

void Menu::addOption(string name, void(*callback)())
{
	addOption(name);
	_callbacks[name] = callback;
}

void Menu::addOptionArrow(string name, void(*leftCallback)(), void(*rightCallback)())
{
	addOption(name);

	ArrowOption arrowOption;
	arrowOption.left = leftCallback;
	arrowOption.right = rightCallback;
	_arrowOptions[name] = arrowOption;
}

void Menu::addOptionClose(string name)
{
	addOption(name);
	_closeOptions.insert(name);
}

void Menu::addOptionWithValues(string name, vector<string> values)
{
	_options.push_back(name);
	_optionsValues[name] = values;
	_optionsValuesChoices[name] = 0;

	if (name.length() > _longestOptionWithChoice)
		_longestOptionWithChoice = name.length();

	for (int i = 0; i < values.size(); i++)
	{
		int totalLength = name.length() + values[i].length() + 4;
		if (totalLength > _longestOption)
			_longestOption = totalLength;

		if (values[i].length() > _longestOptionValue)
			_longestOptionValue = values[i].length();
	}
}

void Menu::addOption(string name)
{
	_options.push_back(name);
	if (name.length() > _longestOption)
		_longestOption = name.length();
}

void Menu::open()
{
	generate();
	//save();
	
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	while (true)
	{
		draw();
		int key = rlutil::getkey();
		switch (key)
		{
		case rlutil::KEY_UP:
			_choice--;
			if (_choice < 0)
				_choice = _options.size() - 1;
			break;
		case rlutil::KEY_DOWN:
			_choice++;
			if (_choice >= _options.size())
				_choice = 0;
			break;
		case rlutil::KEY_LEFT:
			switchOptions(_choice, rlutil::KEY_LEFT);
			break;
		case rlutil::KEY_RIGHT:
			switchOptions(_choice, rlutil::KEY_RIGHT);
			break;
		case rlutil::KEY_ENTER:
			select(_choice);
			break;
		}

		if (_close)
			break;
	}
	clear();
}

void Menu::generate()
{
	_dialog.clear();

	int middleWidth = _longestOption + 4;;
	if (_name.length() > middleWidth)
	{
		middleWidth = _name.length();
	}
	
	if (MINIMUM_INTERIOR_WIDTH > middleWidth)
		middleWidth = MINIMUM_INTERIOR_WIDTH;

	int width = middleWidth + 2;

	_clearLine.clear();
	for (int i = 0; i < width; i++)
		_clearLine.append(" ");

	_dialog.push_back(generateBar("É", "Í", "»", middleWidth));
	_dialog.push_back(generateNameCenter(_name, width));
	_dialog.push_back(generateBar("Ì", "Í", "¹", middleWidth));

	for (int i = 0; i < _options.size(); i++)
	{
		_dialog.push_back(generateOption(_options[i], width));
	}

	_dialog.push_back(generateBar("È", "Í", "¼", middleWidth));

	int windowWidth = 80;
	int windowHeight = 28;
	
	_width = width;
	_height = _dialog.size();
	_x = (windowWidth / 2) - (_width / 2);
	_y = (windowHeight / 2) - (_height / 2);
	_choice = 0;
	_close = false;
}

string Menu::generateOption(string name, int width)
{
	int interiorWidth = width - 2;
	string result = "º";
	for (int i = 0; i < 3; i++)
	{
		result.append(" ");
	}
	result.append(name);

	int nbSpaceEnd = interiorWidth - name.length() - 3;

	if (_optionsValues.find(name) != _optionsValues.end())
	{
		int nbSpaceBeforeSub = _longestOptionWithChoice - name.length();
		for (int i = 0; i < nbSpaceBeforeSub; i++)
			result.append(" ");

		result.append(" : ");

		_optionsValuesChoicesX[name] = result.length();

		nbSpaceEnd -= nbSpaceBeforeSub + 3;
	}

	for (int i = 0; i < nbSpaceEnd; i++)
	{
		result.append(" ");
	}
	result.append("º");

	return result;
}

string Menu::generateNameCenter(string name, int width)
{
	int interiorWidth = width - 2;
	string result = "º";

	int nbSpaceBegin = (interiorWidth / 2) - (name.length() / 2);
	int nbSpaceEnd = interiorWidth - (nbSpaceBegin + name.length());

	for (int i = 0; i < nbSpaceBegin; i++)
		result.append(" ");

	result.append(name);

	for (int i = 0; i < nbSpaceEnd; i++)
		result.append(" ");

	result.append("º");
	
	return result;
}

string Menu::generateBar(const char* start, const char* middle, const char* end, int nbMiddle)
{
	string result = start;
	for (int i = 0; i < nbMiddle; i++)
	{
		result.append(middle);
	}
	result.append(end);
	return result;
}

void Menu::draw()
{
	rlutil::setColor(rlutil::WHITE);
	for (int i = 0; i < _dialog.size(); i++)
	{
		int option = i - 3;
		rlutil::locate(_x, _y + i);
		cout << _dialog[i];

		if (option == _choice)
		{
			rlutil::locate(_x + 2, _y + i);
			cout << ">";
		}

		if (option >= 0 && option < _options.size())
		{
			string name = _options[option];
			if (_optionsValues.find(name) != _optionsValues.end())
			{
				rlutil::locate(_x + _optionsValuesChoicesX[name], _y + i);
				string value = _optionsValues[name][_optionsValuesChoices[name]];
				cout << value;
				int nbSpace = _longestOptionValue - value.length();
				for (int i = 0; i < nbSpace; i++)
					cout << " ";
			}
		}
	}
}

void Menu::save()
{
	_background.clear();
	_backgroundColor.clear();

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CHAR_INFO* buffer = new CHAR_INFO[_width * _height];

	COORD bufferSize;
	bufferSize.X = _width;
	bufferSize.Y = _height;

	COORD bufferCoord;
	bufferCoord.X = 0;
	bufferCoord.Y = 0;

	SMALL_RECT consoleRect;
	consoleRect.Left = _x;
	consoleRect.Top = _y;
	consoleRect.Right = _x + _width - 1;
	consoleRect.Bottom = _y + _height - 1;

	ReadConsoleOutput(out, buffer, bufferSize, bufferCoord, &consoleRect);

	for (int i = 0; i < _height; i++)
	{
		string result = "";
		_backgroundColor.push_back(vector<int>());

		for (int j = 0; j < _width; j++)
		{
			CHAR_INFO info = buffer[(i * _width) + j];
			result += info.Char.AsciiChar;
			_backgroundColor[i].push_back(info.Attributes & (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED));
		}
		_background.push_back(result);
	}
}

void Menu::restore()
{
	for (int i = 0; i < _background.size(); i++)
	{
		rlutil::locate(_x, _y + i);
		for (int j = 0; j < _background[i].size(); j++)
		{
			//rlutil::setColor(_backgroundColor[i][j]);
			//cout << _background[i][j];
			cout << " ";
		}
	}
}

void Menu::clear()
{
	rlutil::setColor(rlutil::WHITE);
	for (int i = 0; i < _dialog.size(); i++)
	{
		rlutil::locate(_x, _y + i);
		cout << _clearLine;
	}
}

void Menu::select(int choice)
{
	string name = _options[choice];
	if (_menus[name] != NULL)
	{
		clear();
		_menus[name]->open();
		draw();
	}
	else if (_callbacks[name] != NULL)
	{
		_callbacks[name]();
	}
	else if (_closeOptions.find(name) != _closeOptions.end())
	{
		_close = true;
	}
}

void Menu::switchOptions(int choice, int key)
{
	string name = _options[choice];
	if (_arrowOptions.find(name) != _arrowOptions.end())
	{
		if (key == rlutil::KEY_LEFT)
			_arrowOptions[name].left();
		else if (key == rlutil::KEY_RIGHT)
			_arrowOptions[name].right();
	}
	else if (_optionsValues.find(name) != _optionsValues.end())
	{
		int valueChoice = _optionsValuesChoices[name];
		if (key == rlutil::KEY_LEFT)
		{
			valueChoice--;
			if (valueChoice < 0)
				valueChoice = _optionsValues[name].size() - 1;
		}
		else if (key == rlutil::KEY_RIGHT)
		{
			valueChoice++;
			if (valueChoice >= _optionsValues[name].size())
				valueChoice = 0;
		}

		_optionsValuesChoices[name] = valueChoice;
	}
}
