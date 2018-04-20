#pragma once

#include "ColoredString.h"

namespace konsole
{
	class ColoredStringIterator
	{
	public:
		ColoredStringIterator();
		ColoredStringIterator(ColoredString& other);

		ColoredStringIterator(const ColoredStringIterator& other);

		ColoredStringIterator& operator=(const ColoredStringIterator& other);

		bool operator==(const ColoredStringIterator& other);
		bool operator!=(const ColoredStringIterator& other);

		ColoredStringIterator& operator ++();
		ColoredStringIterator operator ++(int);

		ColoredChar& operator*();
		ColoredChar* operator->();

	private:
		ColoredString* _coloredString;

		size_t _globalPos;
		size_t _stringsPos;
		size_t _localPos;
	};
}