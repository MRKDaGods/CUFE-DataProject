#pragma once

#include <string>
#include <chrono>
#include <functional>

#include "renderer.h"

namespace ui {
	/// <summary>
	/// A user interface controller responsible of rendering graphs, data, etc
	/// </summary>
	class GUI {
	private:
		/// <summary>
		/// App name
		/// </summary>
		_STD string m_Name;

		/// <summary>
		/// The app renderer
		/// </summary>
		_UI Renderer m_Renderer;

		/// <summary>
		/// Time of last frame execution
		/// </summary>
		_CHRONO system_clock::time_point m_LastFrameTime;

		/// <summary>
		/// Represents a handle to the UI thread
		/// </summary>
		void* m_UIThreadHandle;

		/// <summary>
		/// UI loop external callback
		/// </summary>
		_STD function<void()> m_ExternalCallback;

		/// <summary>
		/// UI rendering loop
		/// </summary>
		void UIRenderLoop();

		/// <summary>
		/// The UI thread entry point
		/// </summary>
		static unsigned long UIThreadEntrypoint(void* param);

	public:
		GUI();
		~GUI();

		void Initialize(const char* name, _UTIL Vector2 screenSize, _STD function<void()> externalCallback);

		/// <summary>
		/// The console renderer
		/// </summary>
		/// <returns></returns>
		Renderer* GetRenderer();

		/// <summary>
		/// Renders a string at the specified position
		/// </summary>
		void DrawString(_UTIL Vector2 pos, _STD wstring text, Color color);

		/// <summary>
		/// Renders a string at the specified position
		/// </summary>
		void DrawString(int x, int y, _STD wstring text, Color color);

		/// <summary>
		/// Draws a line from start to end
		/// </summary>
		void DrawLine(_UTIL Vector2 start, _UTIL Vector2 end, Color color = COLOR_FG_WHITE, wchar_t ch = 0x2588);

		/// <summary>
		/// Draws a line from start to end
		/// </summary>
		void DrawLine(int x1, int y1, int x2, int y2, Color color = COLOR_FG_WHITE, wchar_t ch = 0x2588);

		/// <summary>
		/// Draws a box for the given x y w h
		/// </summary>
		void DrawBox(int x, int y, int w, int h, Color color = COLOR_FG_WHITE, wchar_t wchar = L'═', wchar_t vchar = L'║', const wchar_t* corners = L"╔╗╚╝");

		/// <summary>
		/// Draws a filled box
		/// </summary>
		void DrawBoxFilled(int x, int y, int w, int h, Color color);

		/// <summary>
		/// Renders a button and returns if this button is pressed
		/// </summary>
		bool DrawButton(int x, int y, int w, int h, _STD wstring text, Color color, bool useMinWidth = true);

		/// <summary>
		/// Renders a textbox, and returns the current text
		/// </summary>
		void DrawTextbox(int x, int y, int w, int h, _STD wstring& text, Color color);
	};
}