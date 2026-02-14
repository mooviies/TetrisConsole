#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>

#include "Panel.h"

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

class Menu
{
public:
	explicit Menu(std::string title, std::string subtitle = "");
	~Menu();

	void addOption(const std::string& name, Menu* menu, std::function<void()> preOpen = nullptr);
	void addOption(const std::string& name, std::function<void(OptionChoice)> callback);
	void addOptionAction(const std::string& name, std::function<void()> action);
	void setValueChoice(const std::string& name, const std::string& value);
	void addOptionClose(const std::string& name, std::function<void(OptionChoice)> callback = nullptr);
	void addOptionCloseAllMenu(const std::string& name, std::function<void(OptionChoice)> callback = nullptr);
	void addOptionWithValues(const std::string &name, const std::vector<std::string> &values);
	void setOptionHint(const std::string& name, const std::string& hint);
	void setOptionValueHint(const std::string& name, const std::string& value, const std::string& hint);

	// Global DI hooks, set once at startup before any menu is opened.
	static std::function<bool()> shouldExitGame;
	static std::function<void()> onResize;

	OptionChoice open(bool showSubtitle = false, bool escapeCloses = false);

	std::map<std::string, std::string> generateValues();

protected:
	void addOption(const std::string& name);
	void generate();

	void draw();
	void clear() const;

	void select(int choice);
	void switchOptions(int choice, int key);

private:
	std::string _title;
	std::string _subtitle;
	bool _showSubtitle;

	std::vector<std::string> _options;
	std::map<std::string, Menu*> _menus;
	std::map<std::string, std::function<void()>> _preOpenCallbacks;
	std::map<std::string, std::function<void(OptionChoice)>> _callbacks;
	std::map<std::string, std::function<void()>> _actions;
	std::map<std::string, std::vector<std::string>> _optionsValues;
	std::map<std::string, int> _optionsValuesChoices;
	std::set<std::string> _closeOptions;
	std::set<std::string> _closeAllMenusOptions;

	std::map<std::string, std::string> _optionHints;
	std::map<std::string, std::map<std::string, std::string>> _optionValueHints;
	bool _hasHints = false;
	size_t _longestHint = 0;

	Panel _panel;
	Panel _hintPanel;
	size_t _hintRow = 0;
	std::vector<size_t> _optionRows;

	int _choice;
	size_t _longestOption;
	size_t _longestOptionWithChoice;
	size_t _longestOptionValue;
	int _x;
	int _y;
	int _width;
	int _height;

	bool _close;
	bool _escaped;
};
