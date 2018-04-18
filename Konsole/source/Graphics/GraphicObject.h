#pragma once

#include <string>
#include <vector>
#include <cstddef>

#include "Common.h"
#include "Screen.h"
#include "cstring.h"

namespace konsole
{
	enum class ALIGNEMENT {
		LEFT,
		RIGHT,
		CENTER
	};

	class GraphicObject
	{
	public:
		GraphicObject(ALIGNEMENT alignement);
		~GraphicObject();

		uint getWidth() const { return _width; }
		uint getHeight() const { return _height; }
		const string& getLine(size_t index) const { return index < _object.size() ? _object[index] : ""; }

		void addLine(const string& line);
		void editLine(size_t index, const string& line);

		void draw(Screen& screen, int x, int y) const;
		void addChild(const GraphicObject* child, int rx, int ry);

	private:
		const GraphicObject* _parent;
		vector<const GraphicObject*>  _childs;
		vector<Coordinates> _childsCoordinates;

		ALIGNEMENT _alignement;
		uint _width;
		uint _height;

		std::vector<std::vector<cstring>> _object;
		std::vector<std::string> _objectPrefix;
		std::vector<std::string> _objectPostfix;
	};
}
