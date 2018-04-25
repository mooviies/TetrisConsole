#include <iostream>

#include "ScreenWindows.h"
#include "Input.h"
#include "Table.h"
#include "SymbolsLibrary.h"

using namespace konsole;
using namespace std;

int main()
{
	Screen* screen = new ScreenWindows(80, 20);
	screen->initialize();
	screen->setTitle(TEXT("My Game"));

	Table table(5, 5, Border::DOUBLE, Margin(1), Margin(2));
	screen->draw(5, 5, &table);

	while (!Input::pause());

	return 0;
}

