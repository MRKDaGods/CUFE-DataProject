#include "scheduler_view.h"
#include "scheduler.h"

#define TOOLBAR_HEIGHT 4

namespace core {
	SchedulerView::SchedulerView(Scheduler* scheduler, _UI GUI* ui) : m_Scheduler(scheduler), m_UI(ui), m_ToolbarColor(COL_BG(DARK_RED)), m_ShowingSimModeSelector(false),
		m_ShowingInputGenDialog(false), m_IsPickingInputFile(false) {
	}

	void SchedulerView::RenderActionsToolbar() {
		//render box, top right
		_UTIL Vector2 screenSize = m_UI->GetRenderer()->GetScreenSize();

		int padding = 1;
		int bw = 30; //width
		int bx = VEC_INT_X(screenSize) - bw - padding; //xpos
		int bh = 4;
		int by = 0;

		//draw bg
		m_UI->DrawBoxFilled(bx, by, bw, bh, COL_BG(BLACK));

		int bWidth = (bw - 2 * padding) / 3;

		//button info
		constexpr struct {
			wchar_t sym;
			_UI Color col;
		} buttons[3] = { 
			{
				L'►',
				COL_BG(DARK_GREEN)
			},
			{
				L'║',
				COL_BG(DARK_BLUE)
			},
			{
				L'■',
				COL_BG(DARK_RED)
			}
		};

		for (int i = 0; i < 3; i++) {
			if (m_UI->DrawButton(bx + 2 + i * bWidth + (i + 1) * padding, by + 1, bWidth, bh - 2, _STD wstring(1, buttons[i].sym), COLS(COL_BG(BLACK), buttons[i].col >> 4))) {
				HandleToolbarAction(i, buttons[i].col);
			}
		}

		//draw border
		m_UI->DrawBox(bx, by, bw, bh, COLS(COL_BG(BLACK), COL_FG(CYAN)));

		//render mode selector
		SimulationMode curMode = m_Scheduler->GetSimulationInfo()->GetMode();
		_STD wstring modeStr = SimulationModeToWString(curMode) + (m_ShowingSimModeSelector ? L" ▼ " : L" » ");
		if (m_UI->DrawButton(screenSize.x - modeStr.size() - 2, by + bh + 1, modeStr.size() + 1, 2, modeStr, COLS(COL_BG(WHITE), COL_FG(BLACK)))) {
			m_ShowingSimModeSelector = !m_ShowingSimModeSelector;
		}

		if (m_ShowingSimModeSelector) {
			for (int i = 0; i < 3; i++) {
				SimulationMode mode = (SimulationMode)i;
				bool selected = curMode == mode;

				//invert color if selected
				_UI Color col = COLS(COL_BG(WHITE), COL_FG(BLACK));
				if (selected) {
					col = col ^ 0xff;
				}

				modeStr = SimulationModeToWString(mode) + (selected ? L" √ " : L" ○ ");
				if (m_UI->DrawButton(screenSize.x - modeStr.size() - 2, by + bh + 4 + i * 3, modeStr.size() + 1, 2, modeStr, col)) {
					//m_ShowingSimModeSelector = !m_ShowingSimModeSelector;
					m_Scheduler->GetSimulationInfo()->SetMode(mode);

					//update toolbar color (stopped)
					m_ToolbarColor = buttons[2].col;
				}
			}
		}
	}

	void SchedulerView::RenderStatusbar() {
		_UTIL Vector2 screenSize = m_UI->GetRenderer()->GetScreenSize();

		int h = 1;
		m_UI->DrawBoxFilled(0, screenSize.y - h, screenSize.x, h, m_ToolbarColor);

		const wchar_t* simState = 0;
		switch (m_Scheduler->GetSimulationInfo()->GetState())
		{
		case SimulationState::Stopped:
			simState = L"STOPPED";
			break;

		case SimulationState::Playing:
			simState = L"PLAYING";
			break;

		case SimulationState::Paused:
			simState = L"PAUSED";
			break;
		}

		_STD wstring simMode = SimulationModeToWString(m_Scheduler->GetSimulationInfo()->GetMode());

		wchar_t buf[100];
		swprintf(buf, L"%s (%s)", simMode.c_str(), simState);
		m_UI->DrawString(0, screenSize.y - h / 2.f, buf, COLS(m_ToolbarColor, COL_FG(WHITE)));
	}

	void SchedulerView::RenderMenu() {
		_UTIL Vector2 screenSize = m_UI->GetRenderer()->GetScreenSize();

		//render bg fill
		m_UI->DrawBoxFilled(0, 0, VEC_INT_X(screenSize), 5, COL_BG(BLACK));

		//render title
		m_UI->DrawBoxFilled(0, 0, 18, 4, COL_BG(DARK_GREY));
		m_UI->DrawBox(0, 0, 18, 4, COLS(COL_BG(DARK_GREY), COL_FG(WHITE)), L'/', L'/', L"----");
		m_UI->DrawString(1, 2, L"PROCESS SCHEDULER", COLS(COL_BG(DARK_GREY), COL_FG(WHITE)));

		//menu buttons
		int loadFileWidth = 16;
		int inputGenWidth = 16;
		int totalPadding = 1; //buttonCount - 1

		int totalWidth = inputGenWidth + loadFileWidth + totalPadding;

		int x = screenSize.x / 2.f - totalWidth / 2.f;

		if (m_UI->DrawButton(x, 0, loadFileWidth, 2, L"LOAD INPUT FILE", COLS(COL_BG(BLACK), COL_FG(CYAN)))) {
			m_IsPickingInputFile = !m_IsPickingInputFile;
		}
		x += loadFileWidth + 1;

		//render test input file
		if (m_UI->DrawButton(x, 0, inputGenWidth, 2, L"INPUT GENERATOR", COLS(COL_BG(BLACK), COL_FG(CYAN)))) {
			m_ShowingInputGenDialog = !m_ShowingInputGenDialog;
		}

		//render input file info
		RenderLoadedInputFileInfo();
	}

	void SchedulerView::RenderTestInputDialog() {
		_UTIL Vector2 screenSize = m_UI->GetRenderer()->GetScreenSize();

		//render mask
		m_UI->DrawBoxFilled(0, TOOLBAR_HEIGHT + 1, VEC_INT_X(screenSize), VEC_INT_Y(screenSize), COL_BG(BLACK));

		int w = 60;
		int h = 50;

		int x = screenSize.x / 2.f - w / 2.f;
		int y = screenSize.y / 2.f - h / 2.f;

		m_UI->DrawBoxFilled(x, y, w, h, COLS(COL_BG(BLACK), COL_FG(WHITE))); //fill
		m_UI->DrawBox(x, y, w, h, COLS(COL_BG(BLACK), COL_FG(WHITE))); //frame

		int mid = x + w / 2.f;

		y += 1;

		//title
		m_UI->DrawString(mid - 10, y, L"INPUT FILE GENERATOR", COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 2;

		//filename
		m_UI->DrawString(mid - 4, y, L"Filename", COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 1;

		m_UI->DrawTextbox(mid - 20, y, 40, 2, m_InputFile.filename, COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 4;

		//procs count
		m_UI->DrawString(mid - 7, y, L"Process Count", COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 1;

		m_UI->DrawTextbox(mid - 20, y, 40, 2, m_InputFile.proc_count, COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 4;

		//processors (respectively)
		m_UI->DrawString(mid - 16, y, L"Processor Count (FCFS, SJF, RR)", COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 1;

		m_UI->DrawTextbox(mid - 20, y, 40, 2, m_InputFile.fcfs_count, COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 3;
		m_UI->DrawTextbox(mid - 20, y, 40, 2, m_InputFile.sjf_count, COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 3;
		m_UI->DrawTextbox(mid - 20, y, 40, 2, m_InputFile.rr_count, COLS(COL_BG(BLACK), COL_FG(WHITE)));

		y += 4;

		//attributes
		m_UI->DrawString(mid - 22, y, L"Attributes (RR slice, RTF, MaxW, STL, Fork)", COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 1;

		m_UI->DrawTextbox(mid - 20, y, 40, 2, m_InputFile.rr_slice, COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 3;
		m_UI->DrawTextbox(mid - 20, y, 40, 2, m_InputFile.rtf, COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 3;
		m_UI->DrawTextbox(mid - 20, y, 40, 2, m_InputFile.maxw, COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 3;
		m_UI->DrawTextbox(mid - 20, y, 40, 2, m_InputFile.stl, COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 3;
		m_UI->DrawTextbox(mid - 20, y, 40, 2, m_InputFile.fork, COLS(COL_BG(BLACK), COL_FG(WHITE)));

		y += 4;

		//sig kill
		m_UI->DrawString(mid - 9, y, L"Generate sigkills?", COLS(COL_BG(BLACK), COL_FG(WHITE)));
		y += 1;

		if (m_UI->DrawButton(mid - 2, y, 4, 2, m_InputFile.generate_sigkills ? L"√" : L" ", COLS(COL_BG(BLACK), COL_FG(WHITE)))) {
			m_InputFile.generate_sigkills = !m_InputFile.generate_sigkills;
		}

		y += 4;

		//create button
		if (m_UI->DrawButton(mid - 15, y, 30, 2, L"CREATE FILE", COLS(COL_BG(BLACK), COL_FG(GREEN)), false)) {
			GenerateInputFile(&m_InputFile);

			//generate input file and close dialog
			m_ShowingInputGenDialog = false;
		}
	}

	void SchedulerView::RenderLoadedInputFileInfo() {
		_UTIL Vector2 screenSize = m_UI->GetRenderer()->GetScreenSize();

		if (m_IsPickingInputFile) {
			//render mask
			m_UI->DrawBoxFilled(0, TOOLBAR_HEIGHT + 1, VEC_INT_X(screenSize), VEC_INT_Y(screenSize), COL_BG(BLACK));

			int mid = screenSize.x / 2.f;
			int h = 10;

			int w = 40;

			int x = screenSize.x / 2.f - w / 2.f;
			int y = screenSize.y / 2.f - h / 2.f;

			m_UI->DrawBoxFilled(x, y, w, h, COL_BG(DARK_RED)); //fill
			m_UI->DrawBox(x, y, w, h, COLS(COL_BG(DARK_RED), COL_FG(WHITE))); //frame

			y += 1;

			//title
			m_UI->DrawString(mid - 8, y, L"LOAD INPUT FILE", COLS(COL_BG(DARK_RED), COL_FG(WHITE)));
			y += 2;

			//filename
			m_UI->DrawString(mid - 4, y, L"Filename", COLS(COL_BG(DARK_RED), COL_FG(WHITE)));
			y += 1;

			m_UI->DrawTextbox(mid - 15, y, 30, 2, m_InputFileName, COLS(COL_BG(DARK_RED), COL_FG(WHITE)));
			y += 3;

			//load button
			if (m_UI->DrawButton(mid - 6, y, 12, 2, L"LOAD FILE", COLS(COL_BG(DARK_RED), COL_FG(WHITE)), false)) {
				//load data
				m_Scheduler->LoadSerializedData(m_InputFileName);

				m_IsPickingInputFile = false;
			}

			return;
		}

		//render a small label with filename
		LoadFileInfo* fileInfo = m_Scheduler->GetLoadFileInfo();
		bool empty = fileInfo->filename.empty();
		wchar_t buf[100];
		swprintf(buf, L"INPUT FILE: %s (%s)", (empty ? L"none" : fileInfo->filename.c_str()), (empty ? L"Pick a file to load" : fileInfo->success ? L"SUCCESSFUL" : L"UNSUCCESSFUL"));

		int len = wcslen(buf);
		m_UI->DrawString(screenSize.x / 2.f - len / 2.f, 4, buf, COLS(COL_BG(BLACK), COL_FG(YELLOW)));
	}

	void SchedulerView::RenderProcessorData() {
		_STD wstringstream stream;
		m_Scheduler->Print(stream);

		_STD wstring str;
		for (int i = 0; _STD getline(stream, str, L'\n'); i++) {
			m_UI->DrawString(0, 30 + i, str, COLS(COL_BG(WHITE), COL_FG(BLACK)));
		}
	}

	void SchedulerView::HandleToolbarAction(int i, _UI Color col) {
		SimulationInfo* sim = m_Scheduler->GetSimulationInfo();

		bool success = false;
		switch (i) {
		case 0: //start
			success = sim->Start();
			break;

		case 1:
			success = sim->Pause();
			break;

		case 2:
			success = sim->Stop();
			break;
		}

		//update color only if operation succeeds
		if (success) {
			//set toolbar color
			m_ToolbarColor = col;
		}
	}

	void SchedulerView::UICallback() {
		if (m_UI->DrawButton(20, 10, 20, 4, L"Advance timestep", _UI COLOR_FG_BLACK | _UI COLOR_BG_WHITE)) {
			m_Scheduler->IncrementTimestep();
		}

		wchar_t curTimestepBuf[100];
		swprintf(curTimestepBuf, L"Current Timestep: %d", m_Scheduler->GetSimulationInfo()->GetTimestep());
		m_UI->DrawString(0, 7, curTimestepBuf, _UI COLOR_FG_BLACK | _UI COLOR_BG_WHITE);

		//test input dialog
		if (m_ShowingInputGenDialog) {
			RenderTestInputDialog();
		}

		RenderProcessorData();

		//keep these last, Z-index order
		RenderMenu();
		RenderActionsToolbar();
		RenderStatusbar();
	}
}
