#pragma once

#include <string>

#include "renderer.h"

namespace ui {
	class GUI {
	private:
		Renderer* m_Renderer;

	public:
		GUI(Renderer* renderer);

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