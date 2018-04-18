#pragma once

#include <string>
#include <iostream>

#include "Common.h"

namespace konsole
{
	class cstring : public std::string
	{
	public:
		cstring(Color color = Color::WHITE, Color backgroundColor = Color::BLACK);
		cstring(const std::string& str, Color color = Color::WHITE, Color backgroundColor = Color::BLACK);
		cstring(const std::string& str, size_t pos, size_t len = npos, Color color = Color::WHITE, Color backgroundColor = Color::BLACK);
		cstring(const char* s, Color color = Color::WHITE, Color backgroundColor = Color::BLACK);
		cstring(const char* s, size_t n, Color color = Color::WHITE, Color backgroundColor = Color::BLACK);
		cstring(size_t n, char c, Color color = Color::WHITE, Color backgroundColor = Color::BLACK);

		cstring(const cstring& str);
		cstring(const cstring& str, size_t pos, size_t len);

		cstring& operator=(const std::string& str);
		cstring& operator=(const char* s);
		cstring& operator=(const char c);

		cstring& operator=(const cstring& str);

		void setColor(Color color);
		void setColor(Color color, Color backgroundColor);
		void setBackgroundColor(Color backgroundColor);

		Color getColor() const { return _color; }
		Color getBackgroundColor() const { return _backgroundColor; }

	private:
		Color _color;
		Color _backgroundColor;
	};
}