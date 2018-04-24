#pragma once

#include <xstring>
#include <string>
#include <vector>

#include "Common.h"
#include "ColoredString.h"

namespace konsole
{
	class ColoredString;

	enum class Alignement {
		NONE,
		LEFT,
		RIGHT,
		CENTER
	};

	class GraphicObject
	{
	public:
		GraphicObject();
		~GraphicObject();

		uint getWidth() const { return _width; }
		uint getHeight() const { return _height; }
		const ColoredString& getLine(size_t index, Alignement alignement = Alignement::NONE) const;

		void addLine(const ColoredString& line);
		void editLine(size_t index, const ColoredString& line);
		void removeLine(size_t index);

		size_t getNbChild() const { return _childs.size(); }
		const GraphicObject* getChild(size_t index) const { return index < _childs.size() ? _childs[index] : nullptr; }
		Coordinates getRelativeCoordinates(size_t index) const { return index < _childsRelativePos.size() ? _childsRelativePos[index] : Coordinates(); }

		void addChild(const GraphicObject* child, uint relativeX, uint relativeY);

	private:
		void generateObject();

	private:
		const GraphicObject* _parent;
		std::vector<const GraphicObject*>  _childs;
		std::vector<Coordinates>  _childsRelativePos;

		uint _width;
		uint _height;

		std::vector<ColoredString> _object;
		std::vector<ColoredString> _objectAlignLeft;
		std::vector<ColoredString> _objectAlignRight;
		std::vector<ColoredString> _objectAlignCenter;
	};
}
