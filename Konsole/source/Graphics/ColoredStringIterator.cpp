#include "ColoredStringIterator.h"


using namespace konsole;
using namespace std;

ColoredStringIterator::ColoredStringIterator()
	: _coloredString(nullptr)
{

}

ColoredStringIterator::ColoredStringIterator(ColoredString& other)
	: _coloredString(&other)
{

}

ColoredStringIterator::ColoredStringIterator(const ColoredStringIterator& other)
{
	_coloredString = other._coloredString;
	_globalPos = other._globalPos;
	_stringsPos = other._stringsPos;
	_localPos = other._localPos;
}

ColoredStringIterator& ColoredStringIterator::operator=(const ColoredStringIterator& other)
{
	_coloredString = other._coloredString;
	_globalPos = other._globalPos;
	_stringsPos = other._stringsPos;
	_localPos = other._localPos;
	return *this;
}

bool ColoredStringIterator::operator==(const ColoredStringIterator& other)
{
	return (_coloredString == other._coloredString) && (_globalPos == other._globalPos);
}

bool ColoredStringIterator::operator!=(const ColoredStringIterator& other)
{
	return (_coloredString != other._coloredString) || (_globalPos != other._globalPos);
}

ColoredStringIterator& ColoredStringIterator::operator ++()
{

	return *this;
}

ColoredStringIterator ColoredStringIterator::operator ++(int)
{
	ColoredStringIterator iter(*this);



	return iter;
}

ColoredChar& ColoredStringIterator::operator*()
{
	
}

ColoredChar* ColoredStringIterator::operator->()
{

}