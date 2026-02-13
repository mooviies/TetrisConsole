#pragma once

#include <string>
#include <map>
#include <ctime>

using namespace std;

class Timer
{
public:
	static Timer & instance();

	void startTimer(const string &id);
	void resetTimer(const string &id, double seconds = 0.0);
	void stopTimer(const string &id);
	double getSeconds(const string &id);
	[[nodiscard]] bool exist(const string &id) const;

private:
	Timer();
	~Timer();

	static Timer* _instance;
	map<string, clock_t> _timers;
};

