#include "Utility.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Platform.h"
#include "rlutil.h"

using namespace std;


Utility::Utility()
= default;


Utility::~Utility()
= default;

void Utility::showTitle(const string& sousTitre)
{
	int ox = Platform::offsetX();
	int oy = Platform::offsetY();

	rlutil::locate(1 + ox, 1 + oy);
	cout << "╔══════════════════════════════════════════════════════════════════════════════╗";
	rlutil::locate(1 + ox, 2 + oy);
	cout << "║                                Tetris Console                                ║";
	rlutil::locate(1 + ox, 3 + oy);
	cout << "╠══════════════════════════════════════════════════════════════════════════════╣";
	rlutil::locate(1 + ox, 4 + oy);

	// Build subtitle line: ║ + centered subtitle + ║ (80 chars total)
	cout << "║";
	int longueur = static_cast<int>(sousTitre.length());
	int leadingSpaces = 39 - (longueur / 2);
	for (int i = 0; i < leadingSpaces; i++)
		cout << ' ';
	cout << sousTitre;
	if (longueur % 2 != 0)
		longueur++;
	int trailingSpaces = 39 - (longueur / 2);
	for (int i = 0; i < trailingSpaces; i++)
		cout << ' ';
	cout << "║";

	rlutil::locate(1 + ox, 5 + oy);
	cout << "╚══════════════════════════════════════════════════════════════════════════════╝";
}

string Utility::valueToString(int64_t value, int length)
{
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(length) << value;
	return oss.str();
}
