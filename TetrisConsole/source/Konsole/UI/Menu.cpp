#include "Menu.h"

#include <iostream>
#include <algorithm>
#include <utility>

#include "rlutil.h"
#include "Platform.h"
#include "Color.h"

using namespace std;

std::function<bool()> Menu::shouldExitGame;
std::function<void()> Menu::onResize;

#define MINIMUM_INTERIOR_WIDTH 13

Menu::Menu(string title, string subtitle)
    : _title(std::move(title)), _subtitle(std::move(subtitle)), _showSubtitle(false), _choice(0), _longestOption(0),
      _longestOptionWithChoice(0), _longestOptionValue(0), _x(0), _y(0), _width(0), _height(0), _close(false),
      _escaped(false) {
}


Menu::~Menu() = default;

void Menu::addOption(const string &name, Menu *menu, std::function<void()> preOpen) {
    addOption(name);
    _menus[name] = menu;
    if (preOpen) _preOpenCallbacks[name] = std::move(preOpen);
}

void Menu::addOption(const string &name, std::function<void(OptionChoice)> callback) {
    addOption(name);
    _callbacks[name] = std::move(callback);
}

void Menu::addOptionAction(const string &name, std::function<void()> action) {
    addOption(name);
    _actions[name] = std::move(action);
}

void Menu::addOptionClose(const string &name, std::function<void(OptionChoice)> callback) {
    addOption(name);
    _closeOptions.insert(name);
    _callbacks[name] = std::move(callback);
}

void Menu::addOptionCloseAllMenu(const string &name, std::function<void(OptionChoice)> callback) {
    addOption(name);
    _closeAllMenusOptions.insert(name);
    _callbacks[name] = std::move(callback);
    _closeOptions.insert(name);
}

void Menu::addOptionWithValues(const string &name, const vector<string> &values) {
    _options.push_back(name);
    _optionsValues[name] = values;
    _optionsValuesChoices[name] = 0;

    if (name.length() > _longestOptionWithChoice) _longestOptionWithChoice = name.length();

    for (const auto &value : values) {
        if (const size_t totalLength = name.length() + value.length() + 4; totalLength > _longestOption)
            _longestOption = totalLength;

        if (value.length() > _longestOptionValue) _longestOptionValue = value.length();
    }
}

void Menu::setOptionHint(const string &name, const string &hint) {
    _optionHints[name] = hint;
    _hasHints = true;
    if (hint.length() > _longestHint) _longestHint = hint.length();
}

void Menu::setOptionValueHint(const string &name, const string &value, const string &hint) {
    _optionValueHints[name][value] = hint;
    _hasHints = true;
    if (hint.length() > _longestHint) _longestHint = hint.length();
}

void Menu::addOption(const string &name) {
    _options.push_back(name);
    if (name.length() > _longestOption) _longestOption = name.length();
}

OptionChoice Menu::open(const bool showSubtitle, const bool escapeCloses) {
    _showSubtitle = showSubtitle;
    generate();

    Platform::flushInput();
    while (true) {
        if (!Platform::isTerminalTooSmall()) draw();
        switch (Platform::getKey()) {
            case rlutil::KEY_UP:
                _choice--;
                if (_choice < 0) _choice = static_cast<int>(_options.size()) - 1;
                break;
            case rlutil::KEY_DOWN:
                _choice++;
                if (_choice >= static_cast<int>(_options.size())) _choice = 0;
                break;
            case rlutil::KEY_LEFT: switchOptions(_choice, rlutil::KEY_LEFT); break;
            case rlutil::KEY_RIGHT: switchOptions(_choice, rlutil::KEY_RIGHT); break;
            case rlutil::KEY_ENTER: select(_choice); break;
            case rlutil::KEY_ESCAPE:
                if (escapeCloses) {
                    _choice = 0;
                    _close = true;
                    _escaped = true;
                }
                break;
            default:;
        }

        if (Platform::wasResized()) {
            if (!Platform::isTerminalTooSmall()) {
                generate();
                if (onResize) onResize();
            }
            continue;
        }

        if (_close || (shouldExitGame && shouldExitGame())) break;
    }
    clear();

    const bool exitAllMenu = !_escaped && _closeAllMenusOptions.find(_options[_choice]) != _closeAllMenusOptions.end();
    return {_choice, _options, generateValues(), exitAllMenu};
}

void Menu::generate() {
    size_t middleWidth = _longestOption + 5;
    middleWidth = max(middleWidth, _title.length() + 2);
    if (_showSubtitle) middleWidth = max(middleWidth, _subtitle.length() + 2);
    middleWidth = max(middleWidth, static_cast<size_t>(MINIMUM_INTERIOR_WIDTH));

    _panel = Panel(static_cast<int>(middleWidth));

    _panel.addRow(_title, Align::Center);
    if (_showSubtitle) {
        _panel.addSeparator();
        _panel.addRow(_subtitle, Align::Center);
    }
    _panel.addSeparator();

    _optionRows.clear();
    for (const auto &option : _options) {
        // cppcheck-suppress useStlAlgorithm
        _optionRows.push_back(_panel.addRow("  " + option, Align::Left));
    }

    _width = _panel.width();
    _height = _panel.height();

    constexpr int windowWidth = 80;
    constexpr int windowHeight = 28;
    _x = Platform::offsetX() + 1 + (windowWidth - _width) / 2;
    _y = Platform::offsetY() + 1 + (windowHeight - _height) / 2;
    _panel.setPosition(_x, _y);

    if (_hasHints) {
        size_t hintWidth = max(_longestHint + 2, static_cast<size_t>(MINIMUM_INTERIOR_WIDTH));
        _hintPanel = Panel(static_cast<int>(hintWidth));
        _hintRow = _hintPanel.addRow("", Align::Center, Color::GREY);
        int hintPanelWidth = _hintPanel.width();
        int hx = Platform::offsetX() + 1 + (windowWidth - hintPanelWidth) / 2;
        int hy = _y + _height;
        _hintPanel.setPosition(hx, hy);
    }

    _choice = 0;
    _close = false;
    _escaped = false;
}

map<string, string> Menu::generateValues() {
    map<string, string> values;
    for (const auto &name : _options) {
        if (_optionsValues.find(name) != _optionsValues.end()) {
            values[name] = _optionsValues[name][_optionsValuesChoices[name]];
        }
    }
    return values;
}

void Menu::draw() {
    for (size_t i = 0; i < _options.size(); i++) {
        string text;
        if (static_cast<int>(i) == _choice)
            text = "> ";
        else
            text = "  ";

        text += _options[i];

        if (_optionsValues.find(_options[i]) != _optionsValues.end()) {
            size_t namePad = _longestOptionWithChoice - _options[i].length();
            text.append(namePad, ' ');
            text += " : ";
            text += _optionsValues[_options[i]][_optionsValuesChoices[_options[i]]];
        }

        _panel.setCell(_optionRows[i], 0, text);
    }

    if (_hasHints) {
        string hint;
        const string &name = _options[static_cast<size_t>(_choice)];

        // Check value-specific hint first
        if (auto valHintIt = _optionValueHints.find(name); valHintIt != _optionValueHints.end()) {
            auto valIt = _optionsValues.find(name);
            if (valIt != _optionsValues.end()) {
                const string &currentValue = valIt->second[static_cast<size_t>(_optionsValuesChoices[name])];
                auto it = valHintIt->second.find(currentValue);
                if (it != valHintIt->second.end()) hint = it->second;
            }
        }

        // Fall back to generic hint
        if (hint.empty()) {
            if (const auto it = _optionHints.find(name); it != _optionHints.end()) hint = it->second;
        }

        _hintPanel.setCell(_hintRow, 0, hint);
    }

    _panel.render();
    if (_hasHints) _hintPanel.render();
    cout << flush;
}

void Menu::clear() const {
    _panel.clear();
    if (_hasHints) _hintPanel.clear();
}

void Menu::setValueChoice(const string &name, const string &value) {
    auto valuesIt = _optionsValues.find(name);
    if (valuesIt == _optionsValues.end()) return;
    const auto &values = valuesIt->second;
    for (size_t i = 0; i < values.size(); i++) {
        if (values[i] == value) {
            _optionsValuesChoices[name] = static_cast<int>(i);
            return;
        }
    }
}

void Menu::select(const int choice) {
    string name = _options[choice];

    if (auto it = _actions.find(name); it != _actions.end() && it->second) {
        clear();
        it->second();
        if (onResize) onResize();
        _panel.invalidate();
        if (_hasHints) _hintPanel.invalidate();
        return;
    }

    if (auto it = _menus.find(name); it != _menus.end() && it->second != nullptr) {
        if (auto cb = _preOpenCallbacks.find(name); cb != _preOpenCallbacks.end() && cb->second) cb->second();
        clear();
        if (it->second->open(false, true).exitAllMenus) {
            _close = true;
        } else {
            _panel.invalidate();
            if (_hasHints) _hintPanel.invalidate();
            draw();
        }
    }

    if (auto it = _callbacks.find(name); it != _callbacks.end() && it->second) {
        it->second(OptionChoice(_choice, _options, generateValues()));
    }

    if (_closeOptions.find(name) != _closeOptions.end()) {
        _close = true;
    }
}

void Menu::switchOptions(const int choice, const int key) {
    if (const string name = _options[choice]; _optionsValues.find(name) != _optionsValues.end()) {
        int valueChoice = _optionsValuesChoices[name];
        if (key == rlutil::KEY_LEFT) {
            valueChoice--;
            if (valueChoice < 0) valueChoice = static_cast<int>(_optionsValues[name].size() - 1);
        } else if (key == rlutil::KEY_RIGHT) {
            valueChoice++;
            if (valueChoice >= static_cast<int>(_optionsValues[name].size())) valueChoice = 0;
        }

        _optionsValuesChoices[name] = valueChoice;
    }
}
