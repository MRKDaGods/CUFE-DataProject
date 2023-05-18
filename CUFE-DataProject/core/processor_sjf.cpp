#include "processor_sjf.h"
#include "scheduler.h"

namespace core {
	ProcessorSJF::ProcessorSJF(Scheduler* scheduler) : Processor(ProcessorType::SJF, scheduler) {
	}

	void ProcessorSJF::ScheduleAlgo() {
		//get process from ready
		Process* proc = 0;
		if (m_RunningProcess == 0 && m_ReadyProcesses.Dequeue(&proc)) {
			//run it
			RunProcess(proc);
		}
	}

	void ProcessorSJF::QueueProcess(Process* proc) {
		//update timer
		Processor::QueueProcess(proc);

		//add to ready list
		m_ReadyProcesses.Enqueue(proc);
	}

	void ProcessorSJF::Print(_STD wstringstream& stream) {
		Processor::Print(stream);

		//print ready queue
		stream << m_ReadyProcesses.GetLength() << L" RDY: ";

		//print procs
		m_ReadyProcesses.Print(stream);

		//terminator
		stream << L'\n';
	}

	bool ProcessorSJF::GetStealHandle(StealHandle* stealHandle) {
		if (stealHandle == 0) return false;

		//check for any RDY process
		if (m_ReadyProcesses.GetLength() == 0) return false;

		Process* proc = 0;
		if (!m_ReadyProcesses.Peek(&proc) || proc->IsForked()) return false; //no forked processes are applicable

		//process is applicable for stealing
		*stealHandle = {
			proc,
			[&]() -> void {
				//update timer
				DecrementTimer(proc);

				//remove from queue
				m_ReadyProcesses.Dequeue();
			}
		};

		return true;
	}

	void ProcessorSJF::MigrateAllProcesses() {
		if (m_RunningProcess != 0) {
			DecrementTimer(m_RunningProcess);
			m_Scheduler->Schedule(m_RunningProcess, ProcessorType::None, this);

			m_RunningProcess = 0;
		}

		Process* proc;
		while (m_ReadyProcesses.Dequeue(&proc)) {
			DecrementTimer(proc);
			m_Scheduler->Schedule(proc, ProcessorType::None, this);
		}
	}

	bool ProcessorSJF::IsBusy() {
		return m_RunningProcess || m_ReadyProcesses.GetLength() > 0;
	}
}