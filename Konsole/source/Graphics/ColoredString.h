#pragma once

#include <xstring>
#include <iosfwd>
#include <string>
#include <cstddef>
#include <iostream>
#include <vector>

#include "Common.h"

namespace konsole
{
	struct ColoredChar
	{
		char value = '\0';
		Color textColor = Color::WHITE;
		Color backgroundColor = Color::BLACK;
	};

	ColoredChar toColoredChar(char c, Color textColor = Color::WHITE, Color backgroundColor = Color::BLACK);

	struct ColoredCharTraits : std::char_traits<ColoredChar>
	{
		static constexpr bool eq(char_type c, char_type d) noexcept
		{
			return (c.value == d.value) && (c.textColor == d.textColor) && (c.backgroundColor == d.backgroundColor);
		}

		static constexpr bool lt(char_type c, char_type d) noexcept
		{
			return c.value < d.value;
		}

		static size_t length(const char_type* s)
		{
			size_t len = 0;
			while (s[len].value != '\0')
				len++;

			return len;
		}

		static constexpr char_type to_char_type(int_type c) noexcept
		{
			ColoredChar cc;
			cc.value = c >= CHAR_MIN && c <= CHAR_MAX ? c : '\0';
			return cc;
		}

		static constexpr int_type to_int_type(char_type c) noexcept
		{
			return c.value;
		}
	};

	class ColoredString : public std::basic_string<ColoredChar, ColoredCharTraits>
	{
	public:
		ColoredString();
		ColoredString(char c);
		ColoredString(size_t n, char c);
		ColoredString(const char* str);
		ColoredString(const std::string& str);

		std::string toString() const;
		Color getColor(size_t index) const;
		Color getBackgroundColor(size_t index) const;

		void setColor(Color color);
		void setColor(Color color, size_t index, size_t len);

		void setBackgroundColor(Color color);
		void setBackgroundColor(Color color, size_t index, size_t len);
	};
}