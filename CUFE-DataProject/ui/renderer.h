#pragma once

#include "../common.h"
#include "../utils/vector2.h"
#include "color.h"

#include <string>

//forward decl
struct _CHAR_INFO;
struct _INPUT_RECORD;

namespace ui {
	struct InputState {
		_INPUT_RECORD* records;
		unsigned long evt_count;
	};

	class Renderer {
	private:
		/// <summary>
		/// The std in handle
		/// </summary>
		void* m_StdInHandle;

		/// <summary>
		/// The std out handle
		/// </summary>
		void* m_StdOutHandle;

		/// <summary>
		/// The current screen buffer
		/// </summary>
		_CHAR_INFO* m_ScreenBuffer;

		/// <summary>
		/// Screen buffer size
		/// </summary>
		uint32_t m_ScreenBufferSize;

		/// <summary>
		/// The renderer screen size
		/// </summary>
		_UTIL Vector2 m_ScreenSize;

		/// <summary>
		/// Contains information about the last input state
		/// </summary>
		InputState m_InputState;

		/// <summary>
		/// Last mouse down position
		/// </summary>
		_UTIL Vector2 m_LastMouseDownPos;

	public:
		Renderer();
		~Renderer();

		/// <summary>
		/// Initializes the renderer and the underlying screen buffer
		/// </summary>
		void Initialize(const char* title, _UTIL Vector2 screenSize);

		/// <summary>
		/// Clears the screen buffer
		/// </summary>
		void Clear();

		/// <summary>
		/// Renders the current screen buffer
		/// </summary>
		void Render();

		/// <summary>
		/// Sets a pixel in the screen buffer
		/// </summary>
		void SetPixel(uint32_t x, uint32_t y, wchar_t ch, Color color);

		/// <summary>
		/// Returns the screen size
		/// </summary>
		_UTIL Vector2 GetScreenSize();

		/// <summary>
		/// The stdin handle
		/// </summary>
		void* GetStdInHandle();

		/// <summary>
		/// Returns the last mouse down position
		/// </summary>
		/// <returns></returns>
		_UTIL Vector2 GetLastMouseDownPos();

		/// <summary>
		/// Updates the input state
		/// </summary>
		void UpdateInput();

		/// <summary>
		/// Determines if mouse is down at a certain pos
		/// </summary>
		bool IsMouseDown(int button, int x, int y, int w, int h, _UTIL Vector2* mousePos = 0);

		/// <summary>
		/// Updates the specified buffer w.r.t. to the input state
		/// </summary>
		void UpdateTextBuffer(_STD wstring& buffer);
	};
}