#pragma once

#include "../common.h"
#include "../utils/vector2.h"
#include "color.h"

//forward decl
struct _CHAR_INFO;

namespace ui {
	class Renderer {
	private:
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
	};
}