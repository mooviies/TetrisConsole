#include <iostream>

#include "ScreenWindows.h"
#include "Input.h"

using namespace konsole;
using namespace std;

int main()
{
	Screen* screen = new ScreenWindows(80, 20);
	screen->initialize();
	screen->setTitle(TEXT("My Game"));

	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 80; x++)
		{
			if ((x % 11) == 0)
			{
				screen->draw(x, y, Color::CYAN, 8);
			}
		}
	}

	while (!Input::pause());

	return 0;
}

