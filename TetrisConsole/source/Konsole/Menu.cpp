#include "Menu.h"

#include <utility>

#include "rlutil.h"
#include "Platform.h"

#define MINIMUM_INTERIOR_WIDTH 13

Menu::Menu(string title, string subtitle)
    : _title(std::move(title)), _subtitle(std::move(subtitle)), _showSubtitle(false), _choice(0), _longestOption(0),
      _longestOptionWithChoice(0), _longestOptionValue(0), _x(0), _y(0), _width(0), _height(0), _close(false) {
}

Menu::~Menu() = default;

void Menu::addOption(const string &name, Menu *menu) {
    addOption(name);
    _menus[name] = menu;
}

void Menu::addOption(const string &name, void (*callback)(OptionChoice)) {
    addOption(name);
    _callbacks[name] = callback;
}

void Menu::addOptionArrow(const string& name, void (*leftCallback)(OptionChoice), void (*rightCallback)(OptionChoice)) {
    addOption(name);

    ArrowOption arrowOption;
    arrowOption.left = leftCallback;
    arrowOption.right = rightCallback;
    _arrowOptions[name] = arrowOption;
}

void Menu::addOptionClose(const string& name, void (*callback)(OptionChoice)) {
    addOption(name);
    _closeOptions.insert(name);
    _callbacks[name] = callback;
}

void Menu::addOptionCloseAllMenu(const string& name, void (*callback)(OptionChoice)) {
    addOption(name);
    _closeAllMenusOptions.insert(name);
    _callbacks[name] = callback;
    _closeOptions.insert(name);
}

void Menu::addOptionWithValues(const string& name, const vector<string> &values) {
    _options.push_back(name);
    _optionsValues[name] = values;
    _optionsValuesChoices[name] = 0;

    if (name.length() > _longestOptionWithChoice)
        _longestOptionWithChoice = static_cast<int>(name.length());

    for (const auto & value : values) {
        if (const size_t totalLength = name.length() + value.length() + 4; totalLength > _longestOption)
            _longestOption = static_cast<int>(totalLength);

        if (value.length() > _longestOptionValue)
            _longestOptionValue = static_cast<int>(value.length());
    }
}

void Menu::addOption(const string& name) {
    _options.push_back(name);
    if (name.length() > _longestOption)
        _longestOption = static_cast<int>(name.length());
}

OptionChoice Menu::open(const bool showSubtitle) {
    _showSubtitle = showSubtitle;
    generate();

    Platform::flushInput();
    while (true) {
        draw();
        switch (Platform::getKey()) {
            case rlutil::KEY_UP:
                _choice--;
                if (_choice < 0)
                    _choice = static_cast<int>(_options.size())- 1;
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
            default: ;
        }

        if (_close)
            break;
    }
    clear();

    const bool exitAllMenu = _closeAllMenusOptions.find(_options[_choice]) != _closeAllMenusOptions.end();
    return OptionChoice(_choice, _options, generateValues(), exitAllMenu);
}

void Menu::generate() {
    _dialog.clear();

    int middleWidth = _longestOption + 4;;
    if (_title.length() + 2 > middleWidth) {
        middleWidth = static_cast<int>(_title.length()) + 2;
    }

    if (_subtitle.length() + 2 > middleWidth) {
        middleWidth = static_cast<int>(_subtitle.length()) + 2;
    }

    if (MINIMUM_INTERIOR_WIDTH > middleWidth)
        middleWidth = MINIMUM_INTERIOR_WIDTH;

    const int width = middleWidth + 2;

    _clearLine.clear();
    for (int i = 0; i < width; i++)
        _clearLine.append(" ");

    _dialog.push_back(generateBar("╔", "═", "╗", middleWidth));
    _dialog.push_back(generateNameCenter(_title, width));

    if (_showSubtitle) {
        _dialog.push_back(generateBar("╠", "═", "╣", middleWidth));
        _dialog.push_back(generateNameCenter(_subtitle, width));
    }

    _dialog.push_back(generateBar("╠", "═", "╣", middleWidth));

    for (const auto & _option : _options) {
        _dialog.push_back(generateOption(_option, width));
    }

    _dialog.push_back(generateBar("╚", "═", "╝", middleWidth));

    constexpr int windowWidth = 80;
    constexpr int windowHeight = 28;

    _width = width;
    _height = static_cast<int>(_dialog.size());
    _x = (windowWidth / 2) - (_width / 2);
    _y = (windowHeight / 2) - (_height / 2);
    _choice = 0;
    _close = false;
}

string Menu::generateOption(const string& name, const int width) {
    const int interiorWidth = width - 2;
    string result = "║";
    for (int i = 0; i < 3; i++) {
        result.append(" ");
    }
    result.append(name);

    int nbSpaceEnd = interiorWidth - static_cast<int>(name.length()) - 3;

    if (_optionsValues.find(name) != _optionsValues.end()) {
        const int nbSpaceBeforeSub = _longestOptionWithChoice - static_cast<int>(name.length());
        for (int i = 0; i < nbSpaceBeforeSub; i++)
            result.append(" ");

        result.append(" : ");

        _optionsValuesChoicesX[name] = 1 + 3 + static_cast<int>(name.length()) + nbSpaceBeforeSub + 3;

        nbSpaceEnd -= nbSpaceBeforeSub + 3;
    }

    for (int i = 0; i < nbSpaceEnd; i++) {
        result.append(" ");
    }
    result.append("║");

    return result;
}

string Menu::generateNameCenter(const string& name, const int width) {
    const int interiorWidth = width - 2;
    string result = "║";

    const int nbSpaceBegin = interiorWidth / 2 - static_cast<int>(name.length() / 2);
    const int nbSpaceEnd = interiorWidth - (nbSpaceBegin + static_cast<int>(name.length()));

    for (int i = 0; i < nbSpaceBegin; i++)
        result.append(" ");

    result.append(name);

    for (int i = 0; i < nbSpaceEnd; i++)
        result.append(" ");

    result.append("║");

    return result;
}

string Menu::generateBar(const char *start, const char *middle, const char *end, int nbMiddle) {
    string result = start;
    for (int i = 0; i < nbMiddle; i++) {
        result.append(middle);
    }
    result.append(end);
    return result;
}

map<string, string> Menu::generateValues() {
    map<string, string> values;
    for (const auto& name : _options) {
        if (_optionsValues.find(name) != _optionsValues.end()) {
            values[name] = _optionsValues[name][_optionsValuesChoices[name]];
        }
    }
    return values;
}

void Menu::draw() {
    rlutil::setColor(rlutil::WHITE);
    for (int i = 0; i < _dialog.size(); i++) {
        int option = i;
        if (_showSubtitle)
            option -= 5;
        else
            option -= 3;

        rlutil::locate(_x, _y + i);
        cout << _dialog[i];

        if (option == _choice) {
            rlutil::locate(_x + 2, _y + i);
            cout << ">";
        }

        if (option >= 0 && option < _options.size()) {
            if (string name = _options[option]; _optionsValues.find(name) != _optionsValues.end()) {
                rlutil::locate(_x + _optionsValuesChoicesX[name], _y + i);
                string value = _optionsValues[name][_optionsValuesChoices[name]];
                cout << value;
                const int nbSpace = _longestOptionValue - static_cast<int>(value.length());
                for (int j = 0; j < nbSpace; j++)
                    cout << " ";
            }
        }
    }
    cout << flush;
}

void Menu::save() {
    _background.clear();
    _backgroundColor.clear();
}

void Menu::restore() const {
    for (int i = 0; i < _background.size(); i++) {
        rlutil::locate(_x, _y + i);
        for (int j = 0; j < _background[i].size(); j++) {
            cout << " ";
        }
    }
}

void Menu::clear() const {
    rlutil::setColor(rlutil::WHITE);
    for (int i = 0; i < _dialog.size(); i++) {
        rlutil::locate(_x, _y + i);
        cout << _clearLine;
    }
}

void Menu::select(int choice) {
    string name = _options[choice];
    if (_menus[name] != nullptr) {
        clear();
        if (_menus[name]->open().exitAllMenus) {
            _close = true;
        } else
            draw();
    }

    if (_callbacks[name] != nullptr) {
        _callbacks[name](OptionChoice(_choice, _options, generateValues()));
    }

    if (_closeOptions.find(name) != _closeOptions.end()) {
        _close = true;
    }
}

void Menu::switchOptions(const int choice, const int key) {
    if (const string name = _options[choice]; _arrowOptions.find(name) != _arrowOptions.end()) {
        if (key == rlutil::KEY_LEFT)
            _arrowOptions[name].left(OptionChoice(_choice, _options, generateValues()));
        else if (key == rlutil::KEY_RIGHT)
            _arrowOptions[name].right(OptionChoice(_choice, _options, generateValues()));
    } else if (_optionsValues.find(name) != _optionsValues.end()) {
        int valueChoice = _optionsValuesChoices[name];
        if (key == rlutil::KEY_LEFT) {
            valueChoice--;
            if (valueChoice < 0)
                valueChoice = static_cast<int>(_optionsValues[name].size() - 1);
        } else if (key == rlutil::KEY_RIGHT) {
            valueChoice++;
            if (valueChoice >= _optionsValues[name].size())
                valueChoice = 0;
        }

        _optionsValuesChoices[name] = valueChoice;
    }
}
