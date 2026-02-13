#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>

struct OptionChoice
{
	OptionChoice(const int s, const std::vector<std::string>& o, const std::map<std::string, std::string> &v, const bool e = false)
	{
		selected = s;
		options = o;
		values = v;
		exitAllMenus = e;
	}

	int selected;
	bool exitAllMenus;

	std::vector<std::string> options;
	std::map<std::string, std::string> values;
};

struct ArrowOption
{
	std::function<void(OptionChoice)> left;
	std::function<void(OptionChoice)> right;
};

class Menu
{
public:
	explicit Menu(std::string title, std::string subtitle = "");
	~Menu();

	void addOption(const std::string& name, Menu* menu);
	void addOption(const std::string& name, std::function<void(OptionChoice)> callback);
	void addOptionArrow(const std::string& name, std::function<void(OptionChoice)> leftCallback, std::function<void(OptionChoice)> rightCallback);
	void addOptionClose(const std::string& name, std::function<void(OptionChoice)> callback = nullptr);
	void addOptionCloseAllMenu(const std::string& name, std::function<void(OptionChoice)> callback = nullptr);
	void addOptionWithValues(const std::string &name, const std::vector<std::string> &values);

	static std::function<bool()> shouldExitGame;

	OptionChoice open(bool showSubtitle = false);

protected:
	void addOption(const std::string& name);
	void generate();
	std::string generateOption(const std::string& name, int width);
	static std::string generateNameCenter(const std::string& name, int width);

	static std::string generateBar(const char* start, const char* middle, const char* end, size_t nbMiddle);
	std::map<std::string, std::string> generateValues();

	void draw();
	void save();
	void restore() const;
	void clear() const;

	void select(int choice);
	void switchOptions(int choice, int key);

private:
	std::string _title;
	std::string _subtitle;
	bool _showSubtitle;

	std::vector<std::string> _options;
	std::map<std::string, Menu*> _menus;
	std::map<std::string, std::function<void(OptionChoice)>> _callbacks;
	std::map<std::string, ArrowOption> _arrowOptions;
	std::map<std::string, std::vector<std::string>> _optionsValues;
	std::map<std::string, int> _optionsValuesChoices;
	std::map<std::string, int> _optionsValuesChoicesX;
	std::set<std::string> _closeOptions;
	std::set<std::string> _closeAllMenusOptions;

	std::vector<std::string> _dialog;
	std::string _clearLine;
	std::vector<std::string> _background;
	std::vector<std::vector<int>> _backgroundColor;

	int _choice;
	size_t _longestOption;
	size_t _longestOptionWithChoice;
	size_t _longestOptionValue;
	int _x;
	int _y;
	int _width;
	int _height;

	bool _close;
};
