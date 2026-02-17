#pragma once

#include <string>
#include <cstdint>

class Utility {
public:
    static std::string valueToString(int64_t value, int length);
    static std::string timeToString(double seconds);

    Utility() = delete;
    ~Utility() = delete;
};
