#pragma once

#include <string>
#include <iostream>

using namespace std;

class Utility
{
public:
	static void afficherTitre(string sousTitre);
	static string valueToString(int value, int length);
private:
	Utility();
	~Utility();
};

