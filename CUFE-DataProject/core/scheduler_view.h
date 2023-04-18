#pragma once

#include "../common.h"
#include "input_generator.h"

namespace ui {
	class GUI;
	enum Color;
}

namespace core {
	class Scheduler;
	
	/// <summary>
	/// A renderable view aka the bridge that connects the Scheduler with the UI
	/// </summary>
	class SchedulerView {
	private:
		Scheduler* m_Scheduler;
		_UI GUI* m_UI;

		/// <summary>
		/// Toolbar background color, preferably dark
		/// </summary>
		_UI Color m_ToolbarColor;

		/// <summary>
		/// Is the simulation mode selector expanded?
		/// </summary>
		bool m_ShowingSimModeSelector;

		/// <summary>
		/// Are we displaying the test input dialog?
		/// </summary>
		bool m_ShowingInputGenDialog;

		/// <summary>
		/// An input file model, to generate a new file from
		/// </summary>
		InputFileModel m_InputFile;

		/// <summary>
		/// Are we picking an input file?
		/// </summary>
		bool m_IsPickingInputFile;

		/// <summary>
		/// The currently picked input file name
		/// </summary>
		_STD wstring m_InputFileName;

		/// <summary>
		/// Renders play/pause/stop actions
		/// </summary>
		void RenderActionsToolbar();

		/// <summary>
		/// Renders the statusbar
		/// </summary>
		void RenderStatusbar();

		/// <summary>
		/// Renders the upper menu bar
		/// </summary>
		void RenderMenu();

		/// <summary>
		/// Renders the test input dialog
		/// </summary>
		void RenderTestInputDialog();

		/// <summary>
		/// Renders the currently loaded input file info
		/// </summary>
		void RenderLoadedInputFileInfo();

		void RenderProcessorData();

		/// <summary>
		/// Handles a certain toolbar action
		/// </summary>
		void HandleToolbarAction(int i, _UI Color col);

	public:
		SchedulerView(Scheduler* scheduler, _UI GUI* ui);

		/// <summary>
		/// The external UI callback
		/// </summary>
		void UICallback();
	};
}