#pragma once

#include "GraphicObject.h"

namespace konsole
{
	class Screen
	{
	public:
		Screen(uint width, uint height, Color initialColor = Color::WHITE, Color initialBackgroundColor = Color::BLACK);
		~Screen();

		void initialize();

		bool isInitialized() const;

		uint getWidth() const;
		uint getHeight() const;

		Color getColor() const;
		Color getBackgroundColor() const;

		void setTitle(const std::wstring& title);

		void setColor(Color color);
		void setBackgroundColor(Color color);

		void draw(uint x, uint y, const GraphicObject* obj);

		void draw(uint x, uint y, const ColoredString& str);
		void draw(uint x, uint y, const ColoredChar& c);

		void draw(uint x, uint y, Color color, uint length = 1);

	protected:
		virtual void onInitialize() = 0;

		virtual void onSetTitle(const std::wstring& title) = 0;

		virtual void onSetSize(uint width, uint height) = 0;

		virtual void onSetColor(Color color) = 0;
		virtual void onSetBackgroundColor(Color color) = 0;

		virtual void onSetCursorPosition(uint x, uint y) = 0;

	private:
		void setSize(uint width, uint height);

		void setColor(Color color, bool force);
		void setBackgroundColor(Color color, bool force);

		void setCursorPosition(uint x, uint y);

		bool isValidPosition(uint x, uint y) const;

	private:
		uint _width;
		uint _height;

		Color _currentColor;
		Color _currentBackgroundColor;

		bool _initialized;
	};
}