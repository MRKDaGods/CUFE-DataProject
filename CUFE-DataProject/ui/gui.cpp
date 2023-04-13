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

	void GUI::Initialize(const char* name, _UTIL Vector2 screenSize) {
		m_Name = name;

		//initialize renderer
		m_Renderer.Initialize(name, screenSize);

		//create ui thread
		m_UIThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)UIThreadEntrypoint, this, 0, 0);
	}

	void GUI::DrawString(_UTIL Vector2 pos, _STD wstring text, Color color) {
		for (int i = 0; i < text.size(); i++)
		{
			m_Renderer.SetPixel(pos.x + i, pos.y, text[i], color);
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

	float t = 0;
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

			//render components?

			t += 0.05;

			DrawString(_UTIL Vector2(10, 10), L"Test from ammar and andrew tate", _UI COLOR_FG_WHITE);

			//axis
			DrawLine(_UTIL Vector2(0.f, m_Renderer.GetScreenSize().y / 2.f), _UTIL Vector2(m_Renderer.GetScreenSize().x, m_Renderer.GetScreenSize().y / 2.f), ui::COLOR_FG_WHITE, L'.');

			DrawLine(_UTIL Vector2(m_Renderer.GetScreenSize().x / 2.f, 0.f), _UTIL Vector2(m_Renderer.GetScreenSize().x / 2.f, m_Renderer.GetScreenSize().y), ui::COLOR_FG_WHITE, L'.');

			float angle = M_PI * 5;
			for (float i = 0; i <= angle; i += angle / 100.f) {
				float c = 50 * cosf(i + t);

				auto sz = m_Renderer.GetScreenSize();
				m_Renderer.SetPixel(i * 20, sz.y / 2.f - c, 0x2588, ui::COLOR_FG_MAGENTA);

				float s = 50 * sinf(i + t);
				m_Renderer.SetPixel(i * 20, sz.y / 2.f - s, 0x2588, ui::COLOR_FG_WHITE);

				if (c == 0.f) {
					c = 0.0001f;
				}

				auto t = s / c;
				m_Renderer.SetPixel(i * 10, sz.y / 2.f - t, 0x2588, ui::COLOR_FG_YELLOW);
			}

			//m_GUI.DrawLine(_UTIL Vector2(x, 0.f), _UTIL Vector2(100.f, 50.f));

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