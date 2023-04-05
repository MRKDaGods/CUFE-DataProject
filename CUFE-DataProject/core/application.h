#pragma once

#include <string>
#include <chrono>

#include "../ui/renderer.h"
#include "../ui/gui.h"

namespace core {
	/// <summary>
	/// Main application interface
	/// </summary>
	class Application {
	private:
		/// <summary>
		/// App name
		/// </summary>
		_STD string m_Name;

		/// <summary>
		/// The app renderer
		/// </summary>
		_UI Renderer m_Renderer;

		/// <summary>
		/// The gui interface
		/// </summary>
		_UI GUI m_GUI;

		/// <summary>
		/// Time of last frame execution
		/// </summary>
		_CHRONO system_clock::time_point m_LastFrameTime;

	public:
		Application();

		/// <summary>
		/// Initializes the application
		/// </summary>
		void Initialize(const char* name, _UTIL Vector2 screenSize);

		/// <summary>
		/// Updates the next frame
		/// </summary>
		void Update();
	};
}