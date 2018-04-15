#pragma once

#include "Graphics.h"

namespace konsole
{
	enum GRAPHIC_OBJECT {
		GRAPHIC_OBJECT_RECTANGLE,
		GRAPHIC_OBJECT_TABLE
	};

	class GraphicObject
	{
	protected:
		GraphicObject();
		~GraphicObject();

	private:
		friend class Graphics;
	};
}
