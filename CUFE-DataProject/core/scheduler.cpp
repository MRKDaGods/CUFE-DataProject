#include "scheduler.h"
#include "deserializer.h"
#include "processor_fcfs.h"

namespace core {
	Scheduler::Scheduler() : m_View(this, &m_UI) {
		//initialize ui controller
		m_UI.Initialize(APP_NAME, _UTIL Vector2(APP_SIZE_WIDTH, APP_SIZE_HEIGHT), _STD bind(&SchedulerView::UICallback, &m_View));

		//zero out load file info
		memset(&m_LoadFileInfo, 0, sizeof(LoadFileInfo));
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

	Processor* Scheduler::GetProcessorWithShortestQueue() {
		Processor* proc = *m_Processors[0];
		int val = proc->GetConcurrentTimer();

		for (int i = 1; i < m_Processors.GetLength(); i++) {
			Processor* cur = *m_Processors[i];
			if (cur->GetConcurrentTimer() < val) {
				proc = cur;
				val = cur->GetConcurrentTimer();
			}
		}

		return proc;
	}

	LoadFileInfo* Scheduler::GetLoadFileInfo() {
		return &m_LoadFileInfo;
	}

	SimulationInfo* Scheduler::GetSimulationInfo() {
		return &m_SimulationInfo;
	}

	void Scheduler::Update() {
		//get proc at current timestep
		int ts = m_SimulationInfo.GetTimestep();
		Process* proc = 0;
		while (m_NewProcesses.Peek(&proc) && proc->GetArrivalTime() == ts) {
			//dequeue the proc
			m_NewProcesses.Dequeue();

			//schedule it
			Schedule(proc);
		}

		//update processors
		for (int i = 0; i < m_Processors.GetLength(); i++) {
			(*m_Processors[i])->ScheduleAlgo(this);
		}

		//mark updated
		m_SimulationInfo.NotifyUpdated();
	}

	void Scheduler::Schedule(Process* proc) {
		//get processor with shortest queue
		Processor* processor = GetProcessorWithShortestQueue();
		if (processor == 0) {
			//processor is null? this should never happen
			return;
		}

		//queue process
		processor->QueueProcess(proc);
	}

	void Scheduler::IncrementTimestep() {
		//we can only advance timestep in interactive mode
		if (m_SimulationInfo.GetMode() == SimulationMode::Interactive && m_SimulationInfo.GetState() == SimulationState::Playing) {
			m_SimulationInfo.IncrementTimestep();
		}
	}

	void Scheduler::LoadSerializedData(_STD wstring& filename) {
		//read input file
		Deserializer deserializer(filename);

		DeserializerData data;
		bool success;
		if (success = deserializer.Deserialize(data)) {
			//successfully deserialized data
			for (int i = 0; i < data.proc_count; i++) {
				m_NewProcesses.Enqueue(data.procs[i]);
			}

			//create processors
			//only fcfs for now
			for (int i = 0; i < data.num_processors_fcfs; i++) {
				m_Processors.Add(new ProcessorFCFS());
			}
		}
		
		//update file load info
		m_LoadFileInfo = {
			filename,
			success
		};
	}

	void Scheduler::NotifyProcessTerminated(Process* proc) {
		//add process to TRM list
		m_TerminatedProcesses.Add(proc);
	}

	void Scheduler::NotifyProcessBlocked(Process* proc) {
		//enqueue to BLK
		m_BlockedProcesses.Enqueue(proc);
	}

	void Scheduler::Print(_STD wstringstream& stream) {
		stream << L"Processors\n";

		for (int i = 0; i < m_Processors.GetLength(); i++) {
			(*m_Processors[i])->Print(stream);
		}
	}
}
