#include "gui.h"

namespace ui {
	GUI::GUI(Renderer* renderer) : m_Renderer(renderer) {
	}

	void GUI::DrawString(_UTIL Vector2 pos, _STD wstring text, Color color)
	{
		for (int i = 0; i < text.size(); i++)
		{
			m_Renderer->SetPixel(pos.x + i, pos.y, text[i], color);
		}
	}

	void GUI::DrawLine(_UTIL Vector2 start, _UTIL Vector2 end, Color color, wchar_t ch) {
		//calculate look vector, and render pixels at fixed distances

		auto lookVec = end - start;
		uint32_t dist = (uint32_t)lookVec.magnitude();

		float dx = start.x;
		float dy = start.y;

		for (uint32_t i = 0; i < dist; i++) {
			dx += lookVec.x / dist;
			dy += lookVec.y / dist;

			m_Renderer->SetPixel((uint32_t)dx, (uint32_t)dy, ch, color);
		}
	}
}