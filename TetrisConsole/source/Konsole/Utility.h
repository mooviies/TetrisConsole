#pragma once

#include <string>
#include <cstdint>

class Utility
{
public:
	static void showTitle(const std::string& subtitle);
	static std::string valueToString(int64_t value, int length);

	Utility() = delete;
	~Utility() = delete;
};

