#include "scheduler.h"
#include "deserializer.h"

namespace core {
	Scheduler::Scheduler() {
		//initialize ui controller
		m_UI.Initialize(APP_NAME, _UTIL Vector2(APP_SIZE_WIDTH, APP_SIZE_HEIGHT), _STD bind(&Scheduler::UICallback, this));
		
		//load serialized data
		LoadSerializedData();
	}
	
	Scheduler::~Scheduler() {
		//the underlying implementation of the [] operator iterates about the list
		//till it reaches the provided index, so a for loop would be inefficient
		//in order to prevent unnessecary iterations, we will keep removing the very first element
		//at index 0 aka keep removing the head

		//de alloc processors
		while (!m_Processors.IsEmpty()) {
			Processor* processor = *m_Processors[0];
			m_Processors.Remove(processor);

			delete processor;
		}
	}

	void Scheduler::Update() {
	}

	void Scheduler::Schedule(Process* proc) {
	}

	void Scheduler::LoadSerializedData() {
		//read input file
		Deserializer deserializer("lolxd");

		DeserializerData data;
		if (m_LoadSuccess = deserializer.Deserialize(data)) {
			//successfully deserialized data
			for (int i = 0; i < data.proc_count; i++) {
				m_NewProcesses.Enqueue(data.procs[i]);
			}

			//create processors

		}
	}

	void Scheduler::UICallback() {
		//render header
		m_UI.DrawString(0, 0, L"Process Scheduler", _UI COLOR_BG_CYAN);

		if (m_UI.DrawButton(20, 10, 20, 4, L"Advance timestep", _UI COLOR_FG_BLACK | _UI COLOR_BG_WHITE)) {
			m_CurrentTimestep++;
		}

		//if we didnt successfully load, display error message and dont render anything else
		if (!m_LoadSuccess) {
			m_UI.DrawString(0, 2, L"Cannot load process data", _UI COLOR_BG_DARK_RED | _UI COLOR_FG_WHITE);
			//return;
		}

		wchar_t curTimestepBuf[100];
		swprintf(curTimestepBuf, L"Current Timestep: %d", m_CurrentTimestep);
		m_UI.DrawString(0, 4, curTimestepBuf, _UI COLOR_FG_BLACK | _UI COLOR_BG_WHITE);
	}
}
