#include "processor_fcfs.h"
#include "scheduler.h"

namespace core {
	ProcessorFCFS::ProcessorFCFS() : Processor(ProcessorType::FCFS) {
	}
	
	void ProcessorFCFS::ScheduleAlgo(Scheduler* scheduler) {
		//currently running process is not null, check for completion
		if (m_RunningProcess != 0) {
			m_RunningProcess->Tick();

			//decrement timer by 1 tick
			DecrementTimer();

			//check if proc has finished executing
			if (m_RunningProcess->IsDone()) {
				TerminateRunningProcess(scheduler);
			}
			else if (m_RunningProcess->HasIOEvent(scheduler->GetSimulationInfo()->GetTimestep())) { //check for IO
				//block process
				BlockRunningProcess(scheduler);
			}

			return;
		}

		//get process from ready
		if (m_ReadyProcesses.GetLength() > 0) {
			//running proc should be head O(1)
			Process* proc = *m_ReadyProcesses[0];

			//remove from head O(1)
			m_ReadyProcesses.Remove(proc);

			//run it
			RunProcess(proc);
		}
	}

	void ProcessorFCFS::QueueProcess(Process* proc) {
		//update timer
		Processor::QueueProcess(proc);

		//add to ready list
		m_ReadyProcesses.Add(proc);
	}

	void ProcessorFCFS::Print(_STD wstringstream& stream) {
		Processor::Print(stream);

		//print ready queue
		stream << m_ReadyProcesses.GetLength() << L" RDY: ";

		//print procs
		m_ReadyProcesses.Print(stream);

		//terminator
		stream << L'\n';
	}
}
