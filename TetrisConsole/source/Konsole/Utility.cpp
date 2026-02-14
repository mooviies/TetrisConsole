#include "Utility.h"

#include <sstream>
#include <iomanip>

using namespace std;

string Utility::valueToString(int64_t value, int length){
	ostringstream oss;
	oss << setfill('0') << setw(length) << value;
	return oss.str();
}

string Utility::timeToString(double seconds) {
	ostringstream oss;
	const int minutes = static_cast<int>(seconds / 60.0);
	seconds -= minutes * 60.0;
	oss << setfill('0') << setw(2) << minutes << ":" << setw(2) << static_cast<int>(seconds);
	return oss.str();
}
