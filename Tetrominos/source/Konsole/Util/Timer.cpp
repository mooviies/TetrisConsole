#include "Timer.h"

using namespace std;

Timer &Timer::instance() {
    static Timer t;
    return t;
}

void Timer::startTimer(const string &id) {
    _timers[id] = chrono::steady_clock::now();
}

void Timer::resetTimer(const string &id, const double seconds) {
    const auto now = chrono::steady_clock::now();
    _timers[id] = now - chrono::duration_cast<chrono::steady_clock::duration>(chrono::duration<double>(seconds));
}

void Timer::stopTimer(const string &id) {
    if (_timers.count(id) > 0) _timers.erase(id);
}

double Timer::getSeconds(const string &id) {
    if (_timers.count(id) > 0) {
        const auto now = chrono::steady_clock::now();
        return chrono::duration<double>(now - _timers[id]).count();
    }

    return 0;
}

bool Timer::exist(const string &id) const {
    return _timers.count(id) > 0;
}

Timer::Timer() = default;


Timer::~Timer() = default;
