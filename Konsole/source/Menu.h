#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

namespace konsole
{
	struct OptionChoice
	{
		OptionChoice(int s, std::vector<std::string>& o, std::map<std::string, std::string> v, bool e = false)
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
		Menu(std::string title, std::string subtitle = "");
		~Menu();

		void addOption(std::string name, Menu* menu);
		void addOption(std::string name, void(*callback)(OptionChoice));
		void addOptionArrow(std::string name, void(*leftCallback)(OptionChoice), void(*rightCallback)(OptionChoice));
		void addOptionClose(std::string name, void(*callback)(OptionChoice) = NULL);
		void addOptionCloseAllMenu(std::string name, void(*callback)(OptionChoice) = NULL);
		void addOptionWithValues(std::string name, std::vector<std::string> values);

		OptionChoice open(bool showSubtitle = false);

	protected:
		void addOption(std::string name);
		void generate();
		std::string generateOption(std::string name, int width);
		std::string generateNameCenter(std::string name, int width);
		std::string generateBar(const char* start, const char* middle, const char* end, int nbMiddle);
		std::map<std::string, std::string> generateValues();

		void draw();
		void save();
		void restore();
		void clear();

		void select(int choice);
		void switchOptions(int choice, int key);

	private:
		std::string _title;
		std::string _subtitle;
		bool _showSubtitle;

		std::vector<std::string> _options;
		std::map<std::string, Menu*> _menus;
		std::map<std::string, void(*)(OptionChoice)> _callbacks;
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
		int _longestOption;
		int _longestOptionWithChoice;
		int _longestOptionValue;
		int _x;
		int _y;
		int _width;
		int _height;

		bool _close;
	};
}