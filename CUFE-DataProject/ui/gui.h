#pragma once

#include <string>
#include <chrono>

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

		void Initialize(const char* name, _UTIL Vector2 screenSize);

		/// <summary>
		/// Renders a string at the specified position
		/// </summary>
		void DrawString(_UTIL Vector2 pos, _STD wstring text, Color color);

		/// <summary>
		/// Draws a line from start to end
		/// </summary>
		void DrawLine(_UTIL Vector2 start, _UTIL Vector2 end, Color color = COLOR_FG_WHITE, wchar_t ch = 0x2588);
	};
}