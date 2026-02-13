#pragma once

#include <string>
#include <map>
#include <ctime>

class Timer
{
public:
	static Timer & instance();

	void startTimer(const std::string &id);
	void resetTimer(const std::string &id, double seconds = 0.0);
	void stopTimer(const std::string &id);
	double getSeconds(const std::string &id);
	[[nodiscard]] bool exist(const std::string &id) const;

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

private:
	Timer();
	~Timer();

	std::map<std::string, clock_t> _timers;
};
