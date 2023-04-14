#include "renderer.h"

#include <Windows.h>

namespace ui {
	Renderer::Renderer() : m_StdOutHandle(0), m_ScreenBuffer(0), m_ScreenBufferSize(0) {
	}

	Renderer::~Renderer() {
		//delete screen buffer
		if (m_ScreenBuffer != 0) {
			delete[] m_ScreenBuffer;
		}
	}

	void Renderer::Initialize(const char* title, _UTIL Vector2 screenSize) {
		//update console title
		SetConsoleTitleA(title);

		//set screen size
		m_ScreenSize = screenSize;

		//get stdin handle
		m_StdInHandle = GetStdHandle(STD_INPUT_HANDLE);

		//get stdout handle
		m_StdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		//set screen buffer size
		COORD screenSz = { (short)screenSize.x, (short)screenSize.y };
		SetConsoleScreenBufferSize(m_StdOutHandle, screenSz);

		//update active screen buffer
		SetConsoleActiveScreenBuffer(m_StdOutHandle);

		//update font to consolas (font sz = 9)
		CONSOLE_FONT_INFOEX cfi{};
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = 10;
		cfi.dwFontSize.Y = 15;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		wcscpy_s(cfi.FaceName, L"Consolas");
		SetCurrentConsoleFontEx(m_StdOutHandle, false, &cfi);

		//screen size
		SMALL_RECT screenRect = { 0, 0, (short)screenSize.x - 1, (short)screenSize.y - 1 };
		SetConsoleWindowInfo(m_StdOutHandle, true, &screenRect);	

		//input flags
		SetConsoleMode(m_StdInHandle, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

		//screen buffer
		//calc size
		m_ScreenBufferSize = (uint32_t)(m_ScreenSize.x * m_ScreenSize.y);
		
		//allocate buffer
		m_ScreenBuffer = new CHAR_INFO[m_ScreenBufferSize];
		
		//zero out buffer
		memset(m_ScreenBuffer, 0, m_ScreenBufferSize * sizeof(CHAR_INFO));
	}

	void Renderer::Clear() {
		for (uint32_t i = 0; i < m_ScreenBufferSize; i++) {
			m_ScreenBuffer[i].Char.UnicodeChar = L'\x20';
			m_ScreenBuffer[i].Attributes = COLOR_BG_WHITE;
		}
	}

	void Renderer::Render() {
		//output the current screen buffer

		SMALL_RECT screenRect = { 0, 0, (short)m_ScreenSize.x - 1, (short)m_ScreenSize.y - 1 };
		COORD screenSz = { (short)m_ScreenSize.x, (short)m_ScreenSize.y };

		WriteConsoleOutputW(m_StdOutHandle, m_ScreenBuffer, screenSz, COORD(), &screenRect);
	}

	void Renderer::SetPixel(uint32_t x, uint32_t y, wchar_t ch, Color color) {
		//ensure pixel is within bounds
		if (x >= 0 && x < m_ScreenSize.x && y >= 0 && y < m_ScreenSize.y) {
			//get pixel
			CHAR_INFO* pixel = &m_ScreenBuffer[y * (uint32_t)m_ScreenSize.x + x];
			
			//update pixel
			pixel->Char.UnicodeChar = ch;
			pixel->Attributes = color;
		}
	}

	_UTIL Vector2 Renderer::GetScreenSize() {
		return m_ScreenSize;
	}

	void* Renderer::GetStdInHandle() {
		return m_StdInHandle;
	}

	bool Renderer::IsMouseDown(int button, int x, int y, int w, int h) {
		//read mouse input
		DWORD evtCount;
		GetNumberOfConsoleInputEvents(m_StdInHandle, &evtCount);

		//if there are no input events, return false
		if (evtCount == 0) {
			return false;
		}

		INPUT_RECORD buf[32];
		ReadConsoleInput(m_StdInHandle, buf, evtCount, &evtCount);

		for (DWORD i = 0; i < evtCount; i++) {
			//only check for mouse down event
			INPUT_RECORD evt = buf[i];
			if (evt.EventType == MOUSE_EVENT) {
				if (evt.Event.MouseEvent.dwEventFlags == 0) {
					RECT buttonRect = {
						x, y, x + w, y + h
					};

					COORD mousePos = evt.Event.MouseEvent.dwMousePosition;

					int buttonMask = 1 << button;
					return (evt.Event.MouseEvent.dwButtonState & buttonMask) > 0 && PtInRect(&buttonRect, POINT{ mousePos.X, mousePos.Y });
				}
			}
		}

		return false;
	}
}
