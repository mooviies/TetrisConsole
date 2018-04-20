#pragma once

#include <string>
#include <iostream>
#include <vector>

#include "Common.h"

namespace konsole
{
	struct TextColor
	{
		TextColor(Color textColor = Color::WHITE, Color backgroundColor = Color::BLACK)
		{
			text = textColor;
			background = backgroundColor;
		}

		Color text;
		Color background;
	};

	struct ColoredChar
	{
		ColoredChar(char c = '\0', TextColor textColor = TextColor(Color::WHITE, Color::BLACK))
		{
			value = c;
			color = textColor;
		}

		operator char() const { return value; }

		char value;
		TextColor color;
	};

	class ColoredString
	{
	public:
		/// Constructors

		ColoredString();
		ColoredString(const std::string& str, TextColor color = TextColor());
		ColoredString(const std::string& str, size_t pos, size_t len = string::npos, TextColor color = TextColor());
		ColoredString(const char* s, TextColor color = TextColor());
		ColoredString(const char* s, size_t n, TextColor color = TextColor());
		ColoredString(size_t n, char c, TextColor color = TextColor());

		ColoredString(const ColoredString& str);
		ColoredString(const ColoredString& str, size_t pos, size_t len);
		ColoredString(size_t n, const ColoredChar& c);

		ColoredString& operator=(const ColoredChar& c);
		ColoredString& operator=(const ColoredString& str);

		operator std::string() const;

		/// Capacity

		size_t length() const { return _length; }
		size_t size() const { return _length; }
		bool empty() const { return _length == 0; }

		void resize(size_t n, char c = '\0');
		void clear();

		/// Element Access

		char& operator[] (size_t pos);
		const char& operator[] (size_t pos) const;

		char& at(size_t pos);
		const char& at(size_t pos) const;

		char& back();
		const char& back() const;

		char& front();
		const char& front() const;

		/// Modifiers

		ColoredString& operator+= (const ColoredString& str);
		ColoredString& operator+= (const ColoredChar& ColoredChar);

		string& append(const char* s, size_t n);
		string& append(const ColoredString& str);
		string& append(const ColoredString& str, size_t subpos, size_t sublen);
		string& append(size_t n, const ColoredChar& c);

		void push_back(const ColoredChar& c);

		string& assign(const ColoredString& str);
		string& assign(const ColoredString& str, size_t subpos, size_t sublen);
		string& assign(const char* s, size_t n);
		string& assign(size_t n, const ColoredChar& c);

		string& insert(size_t pos, const string& str);
		string& insert(size_t pos, const string& str, size_t subpos, size_t sublen);
		string& insert(size_t pos, const char* s, size_t n);
		string& insert(size_t pos, size_t n, char c);

		string& erase(size_t pos = 0, size_t len = string::npos);

		string& replace(size_t pos, size_t len, const string& str);

	private:
		size_t _length;

		std::vector<std::string> _strings;
		std::vector<TextColor> _colors;
		std::vector<size_t> _starts;

		const string OUT_OF_RANGE = "Trying to access out of range character in ColoredString";

		friend class ColoredStringIterator;
	};
}