#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

struct ArrowOption
{
	ArrowOption()
	{
		left = NULL;
		right = NULL;
	}
	void(*left)();
	void(*right)();
};

class Menu
{
public:
	Menu(string name);
	~Menu();

	void addOption(string name, Menu* menu);
	void addOption(string name, void(*callback)());
	void addOptionArrow(string name, void(*leftCallback)(), void(*rightCallback)());
	void addOptionClose(string name);
	void addOptionWithValues(string name, vector<string> values);

	void open();

protected:
	void addOption(string name);
	void generate();
	string generateOption(string name, int width);
	string generateNameCenter(string name, int width);
	string generateBar(const char* start, const char* middle, const char* end, int nbMiddle);

	void draw();
	void save();
	void restore();
	void clear();

	void select(int choice);
	void switchOptions(int choice, int key);

private:
	string _name;

	vector<string> _options;
	map<string, Menu*> _menus;
	map<string, void(*)()> _callbacks;
	map<string, ArrowOption> _arrowOptions;
	map<string, vector<string>> _optionsValues;
	map<string, int> _optionsValuesChoices;
	map<string, int> _optionsValuesChoicesX;
	set<string> _closeOptions;

	vector<string> _dialog;
	string _clearLine;
	vector<string> _background;
	vector<vector<int>> _backgroundColor;

	int _choice;
	int _longestOption;
	int _longestOptionWithChoice;
	int _longestOptionValue;
	int _x;
	int _y;
	int _width;
	int _height;

	bool _close;
};

