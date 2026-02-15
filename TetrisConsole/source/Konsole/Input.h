#pragma once

#include <string>
#include <vector>

enum class KeyCode : int {
	// Special keys (256+) — avoid collision with printable ASCII
	ArrowUp = 256,
	ArrowDown,
	ArrowLeft,
	ArrowRight,
	Enter,
	Escape,
	Tab,
	Backspace,
	Shift,
	Control,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	Insert, Delete, Home, End, PageUp, PageDown,
	Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
	Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
	NumpadDel
};

class Input
{
public:
	static void init(int actionCount);
	static void cleanup();
	static void pollKeys();

	static void bind(int action, KeyCode key);
	static void clearBindings(int action);
	static bool action(int action);
	static const std::vector<KeyCode>& getBindings(int action);
	static std::string keyName(KeyCode key);

private:
	static std::vector<std::vector<KeyCode>> s_bindings;
	static std::vector<bool> s_actions;

	Input() = delete;
	~Input() = delete;
};
