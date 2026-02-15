#include "Input.h"

#include <Windows.h>

static int toVK(KeyCode key) {
	int k = static_cast<int>(key);
	// Printable ASCII: letters (uppercase) and digits map directly to VK codes
	if ((k >= 'A' && k <= 'Z') || (k >= '0' && k <= '9'))
		return k;

	switch (key) {
		case KeyCode::ArrowUp:    return VK_UP;
		case KeyCode::ArrowDown:  return VK_DOWN;
		case KeyCode::ArrowLeft:  return VK_LEFT;
		case KeyCode::ArrowRight: return VK_RIGHT;
		case KeyCode::Enter:      return VK_RETURN;
		case KeyCode::Escape:     return VK_ESCAPE;
		case KeyCode::Tab:        return VK_TAB;
		case KeyCode::Backspace:  return VK_BACK;
		case KeyCode::Shift:      return VK_SHIFT;
		case KeyCode::Control:    return VK_CONTROL;
		case KeyCode::F1:         return VK_F1;
		case KeyCode::F2:         return VK_F2;
		case KeyCode::F3:         return VK_F3;
		case KeyCode::F4:         return VK_F4;
		case KeyCode::F5:         return VK_F5;
		case KeyCode::F6:         return VK_F6;
		case KeyCode::F7:         return VK_F7;
		case KeyCode::F8:         return VK_F8;
		case KeyCode::F9:         return VK_F9;
		case KeyCode::F10:        return VK_F10;
		case KeyCode::F11:        return VK_F11;
		case KeyCode::F12:        return VK_F12;
		case KeyCode::Insert:     return VK_INSERT;
		case KeyCode::Delete:     return VK_DELETE;
		case KeyCode::Home:       return VK_HOME;
		case KeyCode::End:        return VK_END;
		case KeyCode::PageUp:     return VK_PRIOR;
		case KeyCode::PageDown:   return VK_NEXT;
		case KeyCode::Numpad0:    return VK_NUMPAD0;
		case KeyCode::Numpad1:    return VK_NUMPAD1;
		case KeyCode::Numpad2:    return VK_NUMPAD2;
		case KeyCode::Numpad3:    return VK_NUMPAD3;
		case KeyCode::Numpad4:    return VK_NUMPAD4;
		case KeyCode::Numpad5:    return VK_NUMPAD5;
		case KeyCode::Numpad6:    return VK_NUMPAD6;
		case KeyCode::Numpad7:    return VK_NUMPAD7;
		case KeyCode::Numpad8:    return VK_NUMPAD8;
		case KeyCode::Numpad9:    return VK_NUMPAD9;
		case KeyCode::NumpadDel:  return VK_DECIMAL;
		default: break;
	}

	// Space
	if (k == ' ')
		return VK_SPACE;

	return 0;
}

void Input::pollKeys() {
	for (size_t a = 0; a < s_bindings.size(); a++) {
		s_actions[a] = false;
		for (KeyCode key : s_bindings[a]) {
			int vk = toVK(key);
			if (vk != 0 && (GetKeyState(vk) & 0x8000)) {
				s_actions[a] = true;
				break;
			}
		}
	}
}
