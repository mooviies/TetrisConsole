#include "GraphicObjDescription.h"

using namespace konsole;
using namespace std;

konsole::GraphicObjDescription::GraphicObjDescription(int rows, int columns, int margins, int padding)
	: _rows(rows), _columns(columns)
{
	_padding = padding;
	for (int i = 0; i < _rows; i++)
	{
		_margins.push_back(vector<Margin>());
		for (int j = 0; j < columns; j++)
		{
			_margins[i].push_back(Margin(margins));
		}
	}
}

GraphicObjDescription::~GraphicObjDescription()
{
}