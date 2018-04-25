#include "ColoredString.h"

#include <exception>

using namespace konsole;
using namespace std;

ColoredChar konsole::toColoredChar(char c, Color textColor, Color backgroundColor)
{
	return ColoredChar(c, textColor, backgroundColor);
}

ColoredString::ColoredString()
{

}

ColoredString::ColoredString(char c)
{
	assign(1, c);
}

ColoredString::ColoredString(size_t n, char c)
{
	assign(n, c);
}

ColoredString::ColoredString(const char* str)
{
	size_t index = 0;
	while (str[index] != '\0')
	{
		append(1, str[index++]);
	}
}

ColoredString::ColoredString(const std::string& str)
{
	for (int i = 0; i < str.length(); i++)
	{
		append(1, str[i]);
	}
}

std::string ColoredString::toString() const
{
	string str;
	for (int i = 0; i < length(); i++)
		str.append(1, at(i).value);
	return str;
}

Color ColoredString::getColor(size_t index) const
{
	return at(index).textColor;
}

Color ColoredString::getBackgroundColor(size_t index) const
{
	return at(index).backgroundColor;
}

ColoredString& ColoredString::setColor(Color color)
{
	for (int i = 0; i < length(); i++)
		at(i).textColor = color;

	return *this;
}

ColoredString& ColoredString::setColor(Color color, size_t index, size_t len)
{
	for (int i = index; i < length() && len > 0; i++, len--)
		at(i).textColor = color;

	return *this;
}

ColoredString& ColoredString::setBackgroundColor(Color color)
{
	for (int i = 0; i < length(); i++)
		at(i).backgroundColor = color;

	return *this;
}

ColoredString& ColoredString::setBackgroundColor(Color color, size_t index, size_t len)
{
	for (int i = index; i < length() && len > 0; i++, len--)
		at(i).backgroundColor = color;

	return *this;
}