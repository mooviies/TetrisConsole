#pragma once

#include <cstdlib>
#include <ctime>

class Random
{
public:
	static void init()
	{
		srand(time(NULL));
	}

	static int getInteger(int min, int max)
	{
		return min + (int)(((max - min) + 1) * rand() / (RAND_MAX + 1.0));
	}

private:
	Random();
	~Random();
};

