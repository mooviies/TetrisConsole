#include "Utility.h"

#include <sstream>
#include <iomanip>

using namespace std;

string Utility::valueToString(const int64_t value, const int length) {
    ostringstream oss;
    oss << setfill('0') << setw(length) << value;
    return oss.str();
}

string Utility::timeToString(const double seconds) {
    const int totalCentis = static_cast<int>(seconds * 100.0);
    const int minutes = totalCentis / 6000;
    const int secs = (totalCentis % 6000) / 100;
    const int centis = totalCentis % 100;
    ostringstream oss;
    oss << setfill('0') << setw(2) << minutes << ":" << setw(2) << secs << "." << setw(2) << centis;
    return oss.str();
}
