#pragma once

namespace konsole
{
	typedef unsigned char byte;
	typedef unsigned short ushort;
	typedef unsigned int uint;
	typedef unsigned long ulong;

	enum class Color
	{
		BLACK,
		BLUE,
		GREEN,
		CYAN,
		RED,
		MAGENTA,
		BROWN,
		GREY,
		DARKGREY,
		LIGHTBLUE,
		LIGHTGREEN,
		LIGHTCYAN,
		LIGHTRED,
		LIGHTMAGENTA,
		YELLOW,
		WHITE
	};

	struct Coordinates
	{
		explicit Coordinates()
		{
			x = 0;
			y = 0;
		}

		explicit Coordinates(uint xv, uint yv)
		{
			x = xv;
			y = yv;
		}

		uint x;
		uint y;
	};
}
