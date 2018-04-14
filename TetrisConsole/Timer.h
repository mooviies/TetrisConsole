#pragma once

#include <string>
#include <map>
#include <time.h>

using namespace std;

class Timer
{
public:
	static Timer & instance();

	void startTimer(string id);
	void resetTimer(string id, double seconds = 0.0);
	void stopTimer(string id);
	double getSeconds(string id);
	bool exist(string id);

private:
	Timer();
	~Timer();

	static Timer* _instance;
	map<string, clock_t> _timers;
};

