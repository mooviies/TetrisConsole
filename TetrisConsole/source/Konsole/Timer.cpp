#include "Timer.h"

using namespace std;

Timer & Timer::instance()
{
	static Timer t;
	return t;
}

void Timer::startTimer(const string& id)
{
	_timers[id] = clock();
}

void Timer::resetTimer(const string& id, const double seconds)
{
	_timers[id] = static_cast<clock_t>(static_cast<double>(clock()) - seconds * static_cast<double>(CLOCKS_PER_SEC));
}

void Timer::stopTimer(const string& id)
{
	if(_timers.count(id) > 0)
		_timers.erase(id);
}

double Timer::getSeconds(const string& id)
{
	if (_timers.count(id) > 0)
	{
		const auto elapsed = static_cast<double>(clock() - _timers[id]);
		return elapsed / static_cast<double>(CLOCKS_PER_SEC);
	}

	return 0;
}

bool Timer::exist(const string& id) const {
	return _timers.count(id) > 0;
}

Timer::Timer()
= default;


Timer::~Timer()
= default;
