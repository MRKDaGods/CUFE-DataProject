#include "renderer.h"

#include <Windows.h>

namespace ui {
	Renderer::Renderer() : m_StdOutHandle(0), m_ScreenBuffer(0), m_ScreenBufferSize(0) {
	}

	Renderer::~Renderer()
	{
		//delete screen buffer
		if (m_ScreenBuffer) {
			delete[] m_ScreenBuffer;
		}
	}

	void Renderer::Initialize(const char* title, _UTIL Vector2 screenSize) {
		//update console title
		SetConsoleTitleA(title);

		//set screen size
		m_ScreenSize = screenSize;

		//get stdout handle
		m_StdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		//set screen buffer size
		COORD screenSz = { (short)screenSize.x, (short)screenSize.y };
		SetConsoleScreenBufferSize(m_StdOutHandle, screenSz);

		//update active screen buffer
		SetConsoleActiveScreenBuffer(m_StdOutHandle);

		//update font to consolas (font sz = 4)
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = 4;
		cfi.dwFontSize.Y = 4;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		wcscpy_s(cfi.FaceName, L"Consolas");
		SetCurrentConsoleFontEx(m_StdOutHandle, false, &cfi);

		//screen size
		SMALL_RECT screenRect = { 0, 0, screenSize.x - 1, screenSize.y - 1 };
		SetConsoleWindowInfo(m_StdOutHandle, true, &screenRect);	

		//input flags
		void* stdInHandle = GetStdHandle(STD_INPUT_HANDLE);
		SetConsoleMode(stdInHandle, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

		//screen buffer
		//calc size
		m_ScreenBufferSize = m_ScreenSize.x * m_ScreenSize.y;
		
		//allocate buffer
		m_ScreenBuffer = new CHAR_INFO[m_ScreenBufferSize];
		
		//zero out buffer
		memset(m_ScreenBuffer, 0, m_ScreenBufferSize * sizeof(CHAR_INFO));
	}

	void Renderer::Clear() {
		for (uint32_t i = 0; i < m_ScreenBufferSize; i++) {
			m_ScreenBuffer[i].Char.UnicodeChar = L'\x20';
		}
	}

	void Renderer::Render() {
		//output the current screen buffer

		SMALL_RECT screenRect = { 0, 0, m_ScreenSize.x - 1, m_ScreenSize.y - 1 };
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
}
