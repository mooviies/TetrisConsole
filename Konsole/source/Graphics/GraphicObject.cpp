#include "GraphicObject.h"

#include "ColoredString.h"

using namespace konsole;
using namespace std;

GraphicObject::GraphicObject(Alignement alignement)
	: _alignement(alignement), _width(0), _height(0), _minimumWidth(0)
{
}

GraphicObject::~GraphicObject()
{
}

const ColoredString& GraphicObject::getLine(size_t index) const
{ 
	if (index >= _object.size())
	{
		return ColoredString("");
	}

	switch (_alignement)
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

void GraphicObject::clear()
{
	_width = 0;
	_height = 0;
	_childs.clear();
	_childsRelativePos.clear();
	_object.clear();
	_objectAlignLeft.clear();
	_objectAlignRight.clear();
	_objectAlignCenter.clear();
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
	_width = _minimumWidth;
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
		uint nbSpace = _width - _object[i].length();
		// LEFT
		_objectAlignLeft.push_back(_object[i]);
		_objectAlignLeft[i].append(nbSpace, toColoredChar(' '));
		
		// RIGHT
		_objectAlignRight.push_back(ColoredString(nbSpace, ' '));
		_objectAlignRight[i].append(_object[i]);

		// CENTER
		uint nbChar = nbSpace / 2;
		uint carry = nbSpace - nbChar * 2;
		ColoredString toAppend = ColoredString(nbChar, ' ');
		_objectAlignCenter.push_back(toAppend);
		_objectAlignCenter[i].append(_object[i]);
		_objectAlignCenter[i].append(toAppend);

		if (carry > 0)
			_objectAlignCenter[i].append(ColoredString(carry, ' '));
	}
}