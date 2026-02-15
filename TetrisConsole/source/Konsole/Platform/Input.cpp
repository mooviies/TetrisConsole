#include "Input.h"

#include <algorithm>

std::vector<std::vector<KeyCode>> Input::s_bindings;
std::vector<bool> Input::s_actions;

void Input::init(const int actionCount) {
	s_bindings.assign(static_cast<size_t>(actionCount), {});
	s_actions.assign(static_cast<size_t>(actionCount), false);
}

void Input::cleanup() {
}

bool Input::action(const int action) {
	return s_actions[static_cast<size_t>(action)];
}

void Input::bind(const int action, const KeyCode key) {
	auto& keys = s_bindings[static_cast<size_t>(action)];
	if (std::find(keys.begin(), keys.end(), key) == keys.end())
		keys.push_back(key);
}

void Input::clearBindings(const int action) {
	s_bindings[static_cast<size_t>(action)].clear();
}

const std::vector<KeyCode>& Input::getBindings(const int action) {
	return s_bindings[static_cast<size_t>(action)];
}

std::string Input::keyName(KeyCode key) {
	int k = static_cast<int>(key);
	if (k >= 'A' && k <= 'Z')
		return std::string(1, static_cast<char>(k));
	if (k >= '0' && k <= '9')
		return std::string(1, static_cast<char>(k));
	if (k == ' ')
		return "Space";

	switch (key) {
		case KeyCode::ArrowUp:    return "Up";
		case KeyCode::ArrowDown:  return "Down";
		case KeyCode::ArrowLeft:  return "Left";
		case KeyCode::ArrowRight: return "Right";
		case KeyCode::Enter:      return "Enter";
		case KeyCode::Escape:     return "Esc";
		case KeyCode::Tab:        return "Tab";
		case KeyCode::Backspace:  return "Bksp";
		case KeyCode::Shift:      return "Shift";
		case KeyCode::Control:    return "Ctrl";
		case KeyCode::F1:         return "F1";
		case KeyCode::F2:         return "F2";
		case KeyCode::F3:         return "F3";
		case KeyCode::F4:         return "F4";
		case KeyCode::F5:         return "F5";
		case KeyCode::F6:         return "F6";
		case KeyCode::F7:         return "F7";
		case KeyCode::F8:         return "F8";
		case KeyCode::F9:         return "F9";
		case KeyCode::F10:        return "F10";
		case KeyCode::F11:        return "F11";
		case KeyCode::F12:        return "F12";
		case KeyCode::Insert:     return "Ins";
		case KeyCode::Delete:     return "Del";
		case KeyCode::Home:       return "Home";
		case KeyCode::End:        return "End";
		case KeyCode::PageUp:     return "PgUp";
		case KeyCode::PageDown:   return "PgDn";
		case KeyCode::Numpad0:    return "Num0";
		case KeyCode::Numpad1:    return "Num1";
		case KeyCode::Numpad2:    return "Num2";
		case KeyCode::Numpad3:    return "Num3";
		case KeyCode::Numpad4:    return "Num4";
		case KeyCode::Numpad5:    return "Num5";
		case KeyCode::Numpad6:    return "Num6";
		case KeyCode::Numpad7:    return "Num7";
		case KeyCode::Numpad8:    return "Num8";
		case KeyCode::Numpad9:    return "Num9";
		case KeyCode::NumpadDel:  return "NumDel";
		default:                  return "?";
	}
}
