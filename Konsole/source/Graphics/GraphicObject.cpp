#include "GraphicObject.h"

using namespace konsole;
using namespace std;

GraphicObject::GraphicObject(ALIGNEMENT alignement)
	: _parent(nullptr), _alignement(alignement), _width(0), _height(0)
{
}

GraphicObject::~GraphicObject()
{
}

void GraphicObject::addLine(const string& line)
{
	_object.push_back(line);
	_height++;

	if (line.size() > _width)
	{
		_width = line.size();
		for (size_t i = 0; i < _object.size(); i++)
		{

		}
	}
}

void GraphicObject::editLine(size_t index, const string& line)
{
	if (index < _object.size())
		_object[index] = line;
}

void GraphicObject::draw(Screen& screen, int x, int y) const
{

}

void GraphicObject::addChild(const GraphicObject* child, int rx, int ry)
{

}