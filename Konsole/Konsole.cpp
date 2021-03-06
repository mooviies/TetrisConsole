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

	GraphicObject object1(Alignement::CENTER);
	object1.addLine(ColoredString("Title").setColor(Color::GREEN).setBackgroundColor(Color::DARKGREY));

	GraphicObject object2(Alignement::CENTER);
	object2.addLine("Ceci est mon Score");
	//object2.addLine("00000000");

	Table table(5, 5, Border::DOUBLE, Margin(1, 1, 0, 0));
	table.addElement(2, 3, &object1);
	table.addElement(0, 3, &object2);

	screen->draw(5, 5, &table);

	while (!Input::pause());

	return 0;
}

