#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

struct OptionChoice
{
	OptionChoice(int s, vector<string>& o, map<string, string> v, bool e = false)
	{
		selected = s;
		options = o;
		values = v;
		exitAllMenus = e;
	}

	int selected;
	bool exitAllMenus;

	vector<string> options;
	map<string, string> values;
};

struct ArrowOption
{
	ArrowOption()
	{
		left = NULL;
		right = NULL;
	}
	void(*left)(OptionChoice);
	void(*right)(OptionChoice);
};

class Menu
{
public:
	Menu(string title, string subtitle = "");
	~Menu();

	void addOption(string name, Menu* menu);
	void addOption(string name, void(*callback)(OptionChoice));
	void addOptionArrow(string name, void(*leftCallback)(OptionChoice), void(*rightCallback)(OptionChoice));
	void addOptionClose(string name, void(*callback)(OptionChoice) = NULL);
	void addOptionCloseAllMenu(string name, void(*callback)(OptionChoice) = NULL);
	void addOptionWithValues(string name, vector<string> values);

	OptionChoice open(bool showSubtitle = false);

protected:
	void addOption(string name);
	void generate();
	string generateOption(string name, int width);
	string generateNameCenter(string name, int width);
	string generateBar(const char* start, const char* middle, const char* end, int nbMiddle);
	map<string, string> generateValues();

	void draw();
	void save();
	void restore();
	void clear();

	void select(int choice);
	void switchOptions(int choice, int key);

private:
	string _title;
	string _subtitle;
	bool _showSubtitle;

	vector<string> _options;
	map<string, Menu*> _menus;
	map<string, void(*)(OptionChoice)> _callbacks;
	map<string, ArrowOption> _arrowOptions;
	map<string, vector<string>> _optionsValues;
	map<string, int> _optionsValuesChoices;
	map<string, int> _optionsValuesChoicesX;
	set<string> _closeOptions;
	set<string> _closeAllMenusOptions;

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

