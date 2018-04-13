#include "stdafx.h"

#include "Timer.h"

Timer* Timer::_instance = NULL;

Timer & Timer::instance()
{
	if (_instance == NULL)
		_instance = new Timer();

	return *_instance;
}

void Timer::startTimer(string id)
{
	_timers[id] = clock();
}

void Timer::resetTimer(string id)
{
	_timers[id] = clock();
}

void Timer::stopTimer(string id)
{
	if(_timers.count(id) > 0)
		_timers.erase(id);
}

double Timer::getSeconds(string id)
{
	if (_timers.count(id) > 0)
		return (clock() - _timers[id]) / (double) CLOCKS_PER_SEC;

	return 0;
}

bool Timer::exist(string id)
{
	return _timers.count(id) > 0;
}

Timer::Timer()
{

}


Timer::~Timer()
{
}
