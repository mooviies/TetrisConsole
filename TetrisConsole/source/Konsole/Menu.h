#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

struct OptionChoice
{
	OptionChoice(const int s, const vector<string>& o, const map<string, string> &v, const bool e = false)
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
		left = nullptr;
		right = nullptr;
	}
	void(*left)(OptionChoice);
	void(*right)(OptionChoice);
};

class Menu
{
public:
	explicit Menu(string title, string subtitle = "");
	~Menu();

	void addOption(const string& name, Menu* menu);
	void addOption(const string& name, void(*callback)(OptionChoice));
	void addOptionArrow(const string& name, void(*leftCallback)(OptionChoice), void(*rightCallback)(OptionChoice));
	void addOptionClose(const string& name, void(*callback)(OptionChoice) = nullptr);
	void addOptionCloseAllMenu(const string& name, void(*callback)(OptionChoice) = nullptr);
	void addOptionWithValues(const string &name, const vector<string> &values);

	OptionChoice open(bool showSubtitle = false);

protected:
	void addOption(const string& name);
	void generate();
	string generateOption(const string& name, int width);
	static string generateNameCenter(const string& name, int width);

	static string generateBar(const char* start, const char* middle, const char* end, int nbMiddle);
	map<string, string> generateValues();

	void draw();
	void save();
	void restore() const;
	void clear() const;

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

