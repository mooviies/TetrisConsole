#pragma once

#include <string>
#include <iostream>

namespace konsole
{
	class Utility
	{
	public:
		static void afficherTitre(std::string sousTitre);
		static std::string valueToString(int value, int length);
	private:
		Utility();
		~Utility();
	};
}