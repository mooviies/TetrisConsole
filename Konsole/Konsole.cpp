#include <iostream>

#include "ScreenWindows.h"

using namespace konsole;
using namespace std;

int main()
{
	Screen* screen = new ScreenWindows(80, 20);
	screen->initialize();

	for (int i = 0; i < 80; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			cout << (i * 80 + j) % 10;
		}
	}

	system("pause");
	return 0;
}

