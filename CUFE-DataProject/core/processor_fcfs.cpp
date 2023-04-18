#include "processor_fcfs.h"
#include "scheduler.h"
#include "random_engine.h"

namespace core {
	ProcessorFCFS::ProcessorFCFS(Scheduler* scheduler) : Processor(ProcessorType::FCFS, scheduler) {
	}
	
	void ProcessorFCFS::ScheduleAlgo() {
		//get process from ready
		if (m_RunningProcess == 0 && m_ReadyProcesses.GetLength() > 0) {
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

	void ProcessorFCFS::KillRandomProcess() {
		if (m_ReadyProcesses.GetLength() == 0) return;

		int pid = RandomEngine::GetInt(1, m_Scheduler->GetLoadFileInfo()->proc_count);
		Process* proc = m_ReadyProcesses.GetProcessWithID(pid);
		if (proc != 0) {
			//remove from ready
			m_ReadyProcesses.Remove(proc);

			//terminate
			TerminateProcess(proc);
		}
	}

	void ProcessorFCFS::ProcessSigkill(int pid) {
		//nothing to do
		if (m_RunningProcess == 0 && m_ReadyProcesses.GetLength() == 0) return;

		//check for running proc first
		if (m_RunningProcess != 0 && m_RunningProcess->GetPID() == pid) {
			//terminate it
			TerminateRunningProcess();
			return;
		}

		//find proc in ready queue
		Process* proc = m_ReadyProcesses.GetProcessWithID(pid);
		if (proc != 0) {
			//remove from ready
			m_ReadyProcesses.Remove(proc);

			//terminate
			TerminateProcess(proc);
		}
	}
}
