#include "ColoredString.h"

#include <exception>

using namespace konsole;
using namespace std;

ColoredString::ColoredString()
	: _length(0)
{
}

ColoredString::ColoredString(const std::string& str, TextColor color)
{
	_strings.push_back(str);
	_colors.push_back(color);
	_starts.push_back(0);
	_length = str.length;
}

ColoredString::ColoredString(const std::string& str, size_t pos, size_t len, TextColor color)
{
	_strings.push_back(string(str, pos, len));
	_colors.push_back(color);
	_starts.push_back(0);
	_length = _strings[0].length;
}

ColoredString::ColoredString(const char* s, TextColor color)
{
	_strings.push_back(string(s));
	_colors.push_back(color);
	_starts.push_back(0);
	_length = _strings[0].length;
}

ColoredString::ColoredString(const char* s, size_t n, TextColor color)
{
	_strings.push_back(string(s, n));
	_colors.push_back(color);
	_starts.push_back(0);
	_length = n;
}

ColoredString::ColoredString(size_t n, char c, TextColor color)
{
	_strings.push_back(string(n, c));
	_colors.push_back(color);
	_starts.push_back(0);
	_length = n;
}

ColoredString::ColoredString(const ColoredString& str)
{
	_strings = str._strings;
	_colors = str._colors;
	_starts = str._starts;
	_length = str._length;
}

ColoredString::ColoredString(const ColoredString& str, size_t pos, size_t len)
{
	const vector<size_t>& starts = str._starts;
	const vector<std::string>& strings = str._strings;
	const vector<TextColor>& colors = str._colors;
	size_t strLength = str.length();
	_length = 0;

	for (size_t i = 0; i < starts.size(); i++)
	{
		size_t begin = starts[i];
		size_t length = strings[i].length();

		if (pos >= begin && pos < strLength)
		{
			size_t s = pos - begin;
			size_t l = len;

			_strings.push_back(strings[i].substr(s, l));
			_colors.push_back(colors[i]);
			_starts.push_back(_length);

			_length += l;

			if (len > length)
				len -= length;
			else
				break;
		}
	}
}

ColoredString::ColoredString(size_t n, const ColoredChar& c)
{
	_strings.push_back(string(n, c));
	_colors.push_back(c.color);
	_starts.push_back(0);
	_length = n;
}

ColoredString& ColoredString::operator=(const ColoredChar& c)
{
	_strings.clear();
	_colors.clear();
	_starts.clear();

	_strings.push_back(string(1, c));
	_colors.push_back(c.color);
	_starts.push_back(0);
	_length = 1;

	return *this;
}

ColoredString& ColoredString::operator=(const ColoredString& str)
{
	_strings = str._strings;
	_colors = str._colors;
	_starts = str._starts;
	_length = str._length;
}

ColoredString::operator std::string() const
{
	string result;
	for (int i = 0; i < _strings.size(); i++)
		result.append(_strings[i]);

	return result;
}

void ColoredString::resize(size_t n, char c = '\0')
{
	if (n > _length)
	{

	}
}

void ColoredString::clear()
{
	_strings.clear();
	_colors.clear();
	_starts.clear();
	_length = 0;
}

char& ColoredString::operator[] (size_t pos)
{
	return this->at(pos);
}

const char& ColoredString::operator[] (size_t pos) const
{
	return this->at(pos);
}

char& ColoredString::at(size_t pos)
{
	for (size_t i = 0; i < _starts.size(); i++)
	{
		size_t begin = _starts[i];
		size_t end = begin + _strings[i].length();

		if (pos >= begin && pos < end)
		{
			size_t s = pos - begin;
			return _strings[i][pos - begin];
		}
	}

	throw out_of_range(OUT_OF_RANGE);
}

const char& ColoredString::at(size_t pos) const
{
	for (size_t i = 0; i < _starts.size(); i++)
	{
		size_t begin = _starts[i];
		size_t end = begin + _strings[i].length();

		if (pos >= begin && pos < end)
		{
			size_t s = pos - begin;
			return _strings[i][pos - begin];
		}
	}

	throw out_of_range(OUT_OF_RANGE);
}

char& ColoredString::back()
{
	if(_length == 0)
		throw out_of_range(OUT_OF_RANGE);

	string& str = _strings[_strings.size() - 1];
	return str[str.length() - 1];
}

const char& ColoredString::back() const
{
	if (_length == 0)
		throw out_of_range(OUT_OF_RANGE);

	const string& str = _strings[_strings.size() - 1];
	return str[str.length() - 1];
}

char& ColoredString::front()
{
	if (_length == 0)
		throw out_of_range(OUT_OF_RANGE);

	return _strings[0][0];
}

const char& ColoredString::front() const
{
	if (_length == 0)
		throw out_of_range(OUT_OF_RANGE);

	return _strings[0][0];
}