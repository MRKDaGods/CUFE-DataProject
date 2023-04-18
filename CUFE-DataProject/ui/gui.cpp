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

	Renderer* GUI::GetRenderer() {
		return &m_Renderer;
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

	void GUI::DrawLine(int x1, int y1, int x2, int y2, Color color, wchar_t ch) {
		DrawLine(_UTIL Vector2(x1, y1), _UTIL Vector2(x2, y2), color, ch);
	}

	void GUI::DrawBox(int x, int y, int w, int h, Color color, wchar_t wchar, wchar_t vchar, const wchar_t* corners) {
		DrawLine(x, y, x + w, y, color, wchar);
		DrawLine(x, y + h, x + w - 1, y + h, color, wchar);

		DrawLine(x, y - 1, x, y + h, color, vchar);
		DrawLine(x + w, y - 1, x + w, y + h, color, vchar);

		m_Renderer.SetPixel(x, y, corners[0], color);
		m_Renderer.SetPixel(x + w, y, corners[1], color);
		m_Renderer.SetPixel(x, y + h, corners[2], color);
		m_Renderer.SetPixel(x + w, y + h, corners[3], color);
	}

	void GUI::DrawBoxFilled(int x, int y, int w, int h, Color color) {
		for (int i = x; i < x + w; i++) {
			for (int j = y; j < y + h; j++) {
				m_Renderer.SetPixel(i, j, L' ', color);
			}
		}
	}

	bool GUI::DrawButton(int x, int y, int w, int h, _STD wstring text, Color color, bool useMinWidth) {
		if (useMinWidth) {
			//width is min of text length w/padding or supplied width
			w = (int)min(w, text.length() + 3);
		}

		//is button clicked?
		bool buttonDown = m_Renderer.IsMouseDown(0, x, y, w, h);

		if (buttonDown) {
			//invert color if button is down
			//xor with 0x00ff
			color = color ^ 0x00ff;
		}
		
		//render button itself
		DrawBox(x, y, w, h, color);

		//render text centered
		int textX = (int)_STD round(x + w / 2.f - text.length() / 2.f);
		int textY = (int)_STD round(y + h / 2.f);
		DrawString(textX, textY, text, color);

		return buttonDown;
	}

	void GUI::DrawTextbox(int x, int y, int w, int h, _STD wstring& text, Color color) {
		//simply draw a button, and alter its text

		//check if we're focused
		RECT tbRect = {
			x, y, x + w, y + h
		};

		_UTIL Vector2 lastDownPos = m_Renderer.GetLastMouseDownPos();
		bool acceptInput = PtInRect(&tbRect, { (LONG)lastDownPos.x, (LONG)lastDownPos.y });

		if (acceptInput) {
			m_Renderer.UpdateTextBuffer(text);
		}

		//the button
		DrawButton(x, y, w, h, text.size() == 0 ? L"..." : text, color, false);
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

			//update input
			m_Renderer.UpdateInput();

			//start rendering

			//clear screen
			m_Renderer.Clear();

			//render external callback
			if (m_ExternalCallback != 0) {
				m_ExternalCallback();
			}

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