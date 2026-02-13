#include "Utility.h"

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;


Utility::Utility()
= default;


Utility::~Utility()
= default;

void Utility::showTitle(const string& sousTitre)
{
	// Cache the subtitle length (reused multiple times below)
	int longueur = static_cast<int>(sousTitre.length());

	// Display the main title frame
	cout << "╔══════════════════════════════════════════════════════════════════════════════╗║"
		<< "                                Tetris Console"
		<< "                                ║╠══════════════════════════════════════════════════════════════════════════════╣"
		<< "║";

	// Print leading spaces to center the subtitle
	for (int i = 0; i < 39 - (longueur / 2); i++)
	{
		cout << ' ';
	}

	// Print the subtitle
	cout << sousTitre;

	// If the subtitle length is odd, bump it up so trailing spaces are symmetric
	if (longueur % 2 != 0)
		longueur++;

	// Print trailing spaces after the subtitle
	for (int i = 0; i < 39 - (longueur / 2); i++)
	{
		cout << ' ';
	}
	// Close the frame
	cout << "║╚══════════════════════════════════════════════════════════════════════════════╝" << endl;
}

string Utility::valueToString(int64_t value, int length)
{
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(length) << value;
	return oss.str();
}
