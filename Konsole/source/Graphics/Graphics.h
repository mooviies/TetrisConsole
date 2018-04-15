#pragma once

#include <string>
#include <rlutil.h>

#include "Screen.h"
#include "GraphicObject.h"

namespace konsole
{
	enum FACTOR {
		FACTOR_ASCII_TO_ANSI = 133
	};

	class Graphics
	{
	public:
		Graphics(Screen& screen);
		~Graphics();

		//GraphicObject* createGraphicObject();

		static char asciiToAnsi(char ascii);
		static std::string asciiToAnsi(const std::string& ascii);

	private:
		Screen & _screen;
		
	};
}
