#pragma once

#include <vector>

namespace konsole
{
	struct Spacing {

		Spacing()
		{
			left = 0;
			right = 0;
			top = 0;
			bottom = 0;
		}

		Spacing(unsigned int value)
		{
			*this = value;
		}

		Spacing& operator=(int value)
		{
			left = right = top = bottom = value;
			return *this;
		}

		unsigned int left;
		unsigned int right;
		unsigned int top;
		unsigned int bottom;
	};

	typedef Spacing Padding;
	typedef Spacing Margin;

	class GraphicObjDescription
	{
	public:
		GraphicObjDescription(int rows = 1, int columns = 1, int margins = 1, int padding = 0);
		~GraphicObjDescription();
	private:
		int _rows;
		int _columns;
		Padding _padding;

		std::vector<std::vector<Margin>> _margins;
	};
}
