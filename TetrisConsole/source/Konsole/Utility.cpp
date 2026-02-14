#include "Utility.h"

#include <sstream>
#include <iomanip>

using namespace std;

string Utility::valueToString(int64_t value, int length)
{
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(length) << value;
	return oss.str();
}
