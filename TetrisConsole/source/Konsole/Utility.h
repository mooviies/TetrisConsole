#pragma once

#include <string>
#include <cstdint>

class Utility
{
public:
	static void showTitle(const std::string& sousTitre);
	static std::string valueToString(int64_t value, int length);
private:
	Utility();
	~Utility();
};

