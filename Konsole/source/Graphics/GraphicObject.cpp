#include "GraphicObject.h"

#include "ColoredString.h"

using namespace konsole;
using namespace std;

GraphicObject::GraphicObject()
	: _parent(nullptr), _width(0), _height(0)
{
}

GraphicObject::~GraphicObject()
{
}

const ColoredString& GraphicObject::getLine(size_t index, Alignement alignement) const
{ 
	if (index >= _object.size())
	{
		if(alignement == Alignement::NONE)
			return ColoredString("");

		return ColoredString(_width, ' ');
	}

	switch (alignement)
	{
	case Alignement::LEFT:
		return _objectAlignLeft[index];
		break;
	case Alignement::RIGHT:
		return _objectAlignRight[index];
		break;
	case Alignement::CENTER:
		return _objectAlignCenter[index];
		break;
	default:
		return _object[index];
		break;
	}
}

void GraphicObject::addLine(const ColoredString& line)
{
	_object.push_back(line);
	_height++;
	generateObject();
}

void GraphicObject::editLine(size_t index, const ColoredString& line)
{
	if (index < _object.size())
	{
		_object[index] = line;
		generateObject();
	}
}

void GraphicObject::removeLine(size_t index)
{
	if (index < _object.size())
	{
		_object.erase(_object.begin() + index);
		_height--;
		generateObject();
	}
}

void GraphicObject::addChild(const GraphicObject* child, uint relativeX, uint relativeY)
{
	bool alreadyChild = false;
	for (size_t i = 0; i < _childs.size(); i++)
	{
		if (_childs[i] == child && _childsRelativePos[i].x == relativeX && _childsRelativePos[i].y == relativeY)
		{
			alreadyChild = true;
			break;
		}
	}

	if (!alreadyChild)
	{
		_childs.push_back(child);
		_childsRelativePos.push_back(Coordinates(relativeX, relativeY));
	}
}

void GraphicObject::generateObject()
{
	_width = 0;
	_objectAlignLeft.clear();
	_objectAlignRight.clear();
	_objectAlignCenter.clear();

	for (size_t i = 0; i < _object.size(); i++)
	{
		if (_object[i].length() > _width)
			_width = _object[i].length();
	}

	for (size_t i = 0; i < _object.size(); i++)
	{
		// LEFT
		_objectAlignLeft.push_back(_object[i]);
		_objectAlignLeft[i].append(_width, toColoredChar(' '));
		
		// RIGHT
		_objectAlignRight.push_back(ColoredString(_width, ' '));
		_objectAlignRight[i].append(_object[i]);

		// CENTER
		uint nbChar = _width / 2;
		uint carry = _width - nbChar;
		ColoredString toAppend = ColoredString(nbChar, ' ');
		_objectAlignCenter.push_back(toAppend);
		_objectAlignCenter[i].append(_object[i]);
		_objectAlignCenter.push_back(toAppend);

		if (carry > 0)
			_objectAlignCenter.push_back(ColoredString(carry, ' '));
	}
}