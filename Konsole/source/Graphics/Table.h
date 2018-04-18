#pragma once

#include "Common.h"

namespace konsole
{
	enum Border {
		SOLID,
		DASHED,
		DOTTED
	};

	struct Spacing {

		Spacing()
		{
			left = 0;
			right = 0;
			top = 0;
			bottom = 0;
		}

		Spacing(uint value)
		{
			*this = value;
		}

		Spacing& operator=(uint value)
		{
			left = right = top = bottom = value;
			return *this;
		}

		uint left;
		uint right;
		uint top;
		uint bottom;
	};

	typedef Spacing Padding;
	typedef Spacing Margin;

	class Table
	{
	public:
		Table();
		~Table();

	private:
	};
}