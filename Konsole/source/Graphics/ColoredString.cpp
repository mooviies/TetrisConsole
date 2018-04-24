#include "ColoredString.h"

#include <exception>

using namespace konsole;
using namespace std;

ColoredChar konsole::toColoredChar(char c, Color textColor, Color backgroundColor)
{
	ColoredChar cc;
	cc.value = c;
	cc.textColor = textColor;
	cc.backgroundColor = backgroundColor;
	return cc;
}

ColoredString::ColoredString()
{

}

ColoredString::ColoredString(char c)
{
	ColoredChar cc;
	cc.value = c;
	assign(1, cc);
}

ColoredString::ColoredString(size_t n, char c)
{
	ColoredChar cc;
	cc.value = c;
	assign(n, cc);
}

ColoredString::ColoredString(const char* str)
{
	size_t index = 0;
	while (str[index] != '\0')
	{
		ColoredChar cc;
		cc.value = str[index++];
		append(1, cc);
	}
}

ColoredString::ColoredString(const std::string& str)
{
	for (int i = 0; i < str.length(); i++)
	{
		ColoredChar cc;
		cc.value = str[i];
		append(1, cc);
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

void ColoredString::setColor(Color color)
{
	for (int i = 0; i < length(); i++)
		at(i).textColor = color;
}

void ColoredString::setColor(Color color, size_t index, size_t len)
{
	for (int i = index; i < length() && len > 0; i++, len--)
		at(i).textColor = color;
}

void ColoredString::setBackgroundColor(Color color)
{
	for (int i = 0; i < length(); i++)
		at(i).backgroundColor = color;
}

void ColoredString::setBackgroundColor(Color color, size_t index, size_t len)
{
	for (int i = index; i < length() && len > 0; i++, len--)
		at(i).backgroundColor = color;
}