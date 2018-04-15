#pragma once

#include <string>
#include <map>
#include <time.h>

namespace konsole
{
	class Timer
	{
	public:
		static Timer & instance();

		void startTimer(std::string id);
		void resetTimer(std::string id, double seconds = 0.0);
		void stopTimer(std::string id);
		double getSeconds(std::string id);
		bool exist(std::string id);

	private:
		Timer();
		~Timer();

		static Timer* _instance;
		std::map<std::string, clock_t> _timers;
	};
}
