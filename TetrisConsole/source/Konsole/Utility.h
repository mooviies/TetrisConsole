#pragma once

#include <string>

using namespace std;

class Utility
{
public:
	static void showTitle(const string& sousTitre);
	static string valueToString(int value, int length);
private:
	Utility();
	~Utility();
};

