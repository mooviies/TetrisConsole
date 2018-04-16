#pragma once

#include <Windows.h>

#include "Screen.h"

namespace konsole
{
	class ScreenWindows : public Screen
	{
	public:
		ScreenWindows(uint width, uint height);
		~ScreenWindows();

	protected:
		virtual void onInitialize() override;

		virtual void onSetTitle(const std::wstring& title) override;

		virtual void onSetSize(uint width, uint height) override;

		virtual void onSetColor(Color color) override;
		virtual void onSetBackgroundColor(Color color) override;

		virtual void onSetCursorPosition(uint x, uint y) override;

	private:
		void hideCursor();

	private:
		HWND _console;
		HWND _desktop;

		HANDLE _output;
		HANDLE _input;

		const uint FONT_WIDTH;
		const uint FONT_HEIGHT;
	};
}