#include "gui.h"

#include <Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

namespace ui {
	GUI::GUI() : m_UIThreadHandle(0) {
		//initialize last frame time to now
		m_LastFrameTime = _CHRONO system_clock::now();
	}

	GUI::~GUI() {
		//terminate ui thread if needed
		if (m_UIThreadHandle != 0) {
#pragma warning(suppress: 6258)
			TerminateThread(m_UIThreadHandle, 0);

			//close the handle
			CloseHandle(m_UIThreadHandle);

			m_UIThreadHandle = 0;
		}
	}

	void GUI::Initialize(const char* name, _UTIL Vector2 screenSize, _STD function<void()> externalCallback) {
		m_Name = name;

		//set external callback
		m_ExternalCallback = externalCallback;

		//initialize renderer
		m_Renderer.Initialize(name, screenSize);

		//create ui thread
		m_UIThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)UIThreadEntrypoint, this, 0, 0);
	}

	void GUI::DrawString(_UTIL Vector2 pos, _STD wstring text, Color color) {
		DrawString((int)pos.x, (int)pos.y, text, color);
	}

	void GUI::DrawString(int x, int y, _STD wstring text, Color color) {
		for (int i = 0; i < text.size(); i++) {
			m_Renderer.SetPixel(x + i, y, text[i], color);
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

			m_Renderer.SetPixel((uint32_t)dx, (uint32_t)dy, ch, color);
		}
	}

	bool GUI::DrawButton(int x, int y, int w, int h, _STD wstring text, Color color) {
		//width is min of text length w/padding or supplied width
		w = min(w, text.length() + 3);

		//is button clicked?
		bool buttonDown = m_Renderer.IsMouseDown(0, x, y, w, h);

		if (buttonDown) {
			//invert color if button is down
			//xor with 0x00ff
			color = color ^ 0x00ff;
		}
		
		//render button itself
		DrawLine(_UTIL Vector2(x, y), _UTIL Vector2(x + w, y), color, L'.');
		DrawLine(_UTIL Vector2(x, y + h), _UTIL Vector2(x + w, y + h), color, L'.');

		DrawLine(_UTIL Vector2(x, y - 1), _UTIL Vector2(x, y + h), color, L'.');
		DrawLine(_UTIL Vector2(x + w, y), _UTIL Vector2(x + w, y + h), color, L'.');

		//render text centered
		int textX = (int)_STD round(x + w / 2.f - text.length() / 2.f);
		int textY = (int)_STD round(y + h / 2.f);
		DrawString(textX, textY, text, color);

		return buttonDown;
	}

	float t = 0;

	void GUI::draw(int x, int y, int w, int h) {
		//DrawLine(_UTIL Vector2(x, y + h / 2.f), _UTIL Vector2(x + w, y + h / 2.f), ui::COLOR_FG_WHITE, L'.');

		//DrawLine(_UTIL Vector2(x + w / 2.f, y), _UTIL Vector2(x + w / 2.f, y + h), ui::COLOR_FG_WHITE, L'.');

		float angle = M_PI * 5;
		for (float i = 0; i <= angle; i += angle / 100.f) {
			float c = 10 * cosf(i + t);

			auto sz = _UTIL Vector2(w, h);
			m_Renderer.SetPixel(x + i * 8, y + sz.y / 2.f - c, 0x2588, ui::COLOR_FG_MAGENTA | _UI COLOR_BG_WHITE);

			float s = 10 * sinf(i + t);
			m_Renderer.SetPixel(x + i * 8, y + sz.y / 2.f - s, 0x2588, ui::COLOR_FG_BLACK | _UI COLOR_BG_WHITE);

			if (c == 0.f) {
				c = 0.0001f;
			}

			auto t = s / c;
			//m_Renderer.SetPixel(i * 10, sz.y / 2.f - t, 0x2588, ui::COLOR_FG_YELLOW);
		}
	}

	void GUI::UIRenderLoop() {
		while (true) {
			//calculate fps, fps=1/elapsed time
			auto time = _CHRONO system_clock::now();

			//get time elapsed since last frame in seconds
			auto elapsed = ((_CHRONO duration<float>)(time - m_LastFrameTime)).count();

			//update last frame time
			m_LastFrameTime = time;

			float fps = 1.f / elapsed;

			//start rendering

			//clear screen
			m_Renderer.Clear();

			//render external callback
			if (m_ExternalCallback != 0) {
				m_ExternalCallback();
			}

			t += 0.05;

			draw(60, 2, 10, 30);

			//update title w/fps
			char title[256];
			sprintf(title, "%s | FPS: %.2f", m_Name.c_str(), fps);
			SetConsoleTitleA(title);

			//render!
			m_Renderer.Render();

			Sleep(10);
		}
	}

	unsigned long GUI::UIThreadEntrypoint(void* param) {
		((GUI*)param)->UIRenderLoop();
		return 0;
	}
}