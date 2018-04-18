#include "cstring.h"

using namespace konsole;
using namespace std;

cstring::cstring(Color color, Color backgroundColor)
	: _color(color), _backgroundColor(backgroundColor)
{
}

cstring::cstring(const std::string& str, Color color, Color backgroundColor)
	: string(str), _color(color), _backgroundColor(backgroundColor)
{

}

cstring::cstring(const std::string& str, size_t pos, size_t len, Color color, Color backgroundColor)
	: string(str, pos, len), _color(color), _backgroundColor(backgroundColor)
{

}

cstring::cstring(const char* s, Color color, Color backgroundColor)
	: string(s), _color(color), _backgroundColor(backgroundColor)
{

}

cstring::cstring(const char* s, size_t n, Color color, Color backgroundColor)
	: string(s, n), _color(color), _backgroundColor(backgroundColor)
{

}

cstring::cstring(size_t n, char c, Color color, Color backgroundColor)
	: string(n, c), _color(color), _backgroundColor(backgroundColor)
{

}

cstring::cstring(const cstring& str)
	: string(str), _color(str._color), _backgroundColor(str._backgroundColor)
{

}

cstring::cstring(const cstring& str, size_t pos, size_t len)
	: string(str, pos, len), _color(str._color), _backgroundColor(str._backgroundColor)
{

}

cstring& cstring::operator=(const string& str)
{
	cstring& ts = (cstring&)string::operator=(str);
	ts._color = _color;
	ts._backgroundColor = _backgroundColor;
	return ts;
}

cstring& cstring::operator=(const char* s)
{
	cstring& ts = (cstring&)string::operator=(s);
	ts._color = _color;
	ts._backgroundColor = _backgroundColor;
	return ts;
}

cstring& cstring::operator=(const char c)
{
	cstring& ts = (cstring&)string::operator=(c);
	ts._color = _color;
	ts._backgroundColor = _backgroundColor;
	return ts;
}

cstring& cstring::operator=(const cstring& str)
{
	cstring& ts = (cstring&)string::operator=(str);
	ts._color = _color;
	ts._backgroundColor = _backgroundColor;
	return ts;
}

void cstring::setColor(Color color)
{
	_color = color;
}

void cstring::setColor(Color color, Color backgroundColor)
{
	_color = color;
	_backgroundColor = backgroundColor;
}

void cstring::setBackgroundColor(Color backgroundColor)
{
	_backgroundColor = backgroundColor;
}