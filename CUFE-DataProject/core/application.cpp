#include "application.h"

#include <Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

namespace core {
	Application::Application() : m_GUI(&m_Renderer) {
		//initialize last frame time to now
		m_LastFrameTime = _CHRONO system_clock::now();
	}

	void core::Application::Initialize(const char* name, _UTIL Vector2 screenSize) {
		m_Name = name;

		//initialize renderer
		m_Renderer.Initialize(name, screenSize);
	}

	float t = 0.f;
	void Application::Update() {
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

		//axis
		m_GUI.DrawLine(_UTIL Vector2(0.f, m_Renderer.GetScreenSize().y / 2.f), _UTIL Vector2(m_Renderer.GetScreenSize().x, m_Renderer.GetScreenSize().y / 2.f), ui::COLOR_FG_WHITE, L'.');

		m_GUI.DrawLine(_UTIL Vector2(m_Renderer.GetScreenSize().x / 2.f, 0.f), _UTIL Vector2(m_Renderer.GetScreenSize().x / 2.f, m_Renderer.GetScreenSize().y), ui::COLOR_FG_WHITE, L'.');
		
		float angle = M_PI * 5;
		for (float i = 0; i <= angle; i += angle / 100.f) {
			float p = 50 * cosf(i + t);

			auto sz = m_Renderer.GetScreenSize();
			m_Renderer.SetPixel(i * 20, sz.y / 2.f - p, 0x2588, ui::COLOR_FG_MAGENTA);

			float z = 50 * sinf(i + t);
			m_Renderer.SetPixel(i * 20, sz.y / 2.f - z, 0x2588, ui::COLOR_FG_WHITE);
		}

		//m_GUI.DrawLine(_UTIL Vector2(x, 0.f), _UTIL Vector2(100.f, 50.f));

		//update title w/fps
		char title[256];
		sprintf(title, "%s | FPS: %.2f", m_Name.c_str(), fps);
		SetConsoleTitleA(title);

		//render!
		m_Renderer.Render();
	}
}