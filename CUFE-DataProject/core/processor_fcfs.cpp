#include "processor_fcfs.h"
#include "scheduler.h"
#include "random_engine.h"

namespace core {
	_COLLECTION LinkedQueue<SigkillTimeInfo> ProcessorFCFS::ms_Sigkills;

	ProcessorFCFS::ProcessorFCFS(Scheduler* scheduler) : Processor(ProcessorType::FCFS, scheduler) {
	}
	
	void ProcessorFCFS::ScheduleAlgo() {
		if (m_RunningProcess != 0) {
			//check for forking
			//the process must have not forked before
			if (!m_RunningProcess->GetForkingData()->has_forked) {
				//generate probability
				int num = RandomEngine::GetInt(1, 100);
				if (num <= m_Scheduler->GetLoadFileInfo()->fork_prob) {
					//fork !!!
					m_Scheduler->ForkProcess(m_RunningProcess);
				}
			}
		}

		//get process from ready
		if (m_RunningProcess == 0 && m_ReadyProcesses.GetLength() > 0) {
			//running proc should be head O(1)
			Process* proc = *m_ReadyProcesses[0];

			//remove from head O(1)
			m_ReadyProcesses.Remove(proc);

			//run it
			RunProcess(proc);
		}

		LOG(L"Looking for sigkill");

		//process sigkill for FCFS
		SigkillTimeInfo sigkill;
		if (ms_Sigkills.Peek(&sigkill) && sigkill.time == m_Scheduler->GetSimulationInfo()->GetTimestep()) {
			//dequeue sigkill
			ms_Sigkills.Dequeue();

			LOGF(L"Found sigkill for proc pid=%d", sigkill.proc_pid);

			//process it
			ProcessSigkill(sigkill.proc_pid);
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
		LOG(L"Attempting to kill a random process");

		if (m_ReadyProcesses.GetLength() == 0) {
			LOG(L"RDY empty, cant kill");
			return;
		}

		int pid = RandomEngine::GetInt(1, m_Scheduler->GetLoadFileInfo()->proc_count);

		LOGF(L"Chosen pid=%d", pid);

		Process* proc = m_ReadyProcesses.GetProcessWithID(pid);
		if (proc != 0) {
			LOG(L"Process to kill found, removing...");

			//remove from ready
			m_ReadyProcesses.Remove(proc);

			//terminate
			TerminateProcess(proc);
		}
		else {
			LOG(L"Process not found");
		}
	}

	void ProcessorFCFS::ProcessSigkill(int pid) {
		LOGF(L"ProcessSigkill(%d)", pid);

		//nothing to do
		if (m_RunningProcess == 0 && m_ReadyProcesses.GetLength() == 0) {
			LOG(L"No running proc & empty RDY");
			return;
		}

		//check for running proc first
		if (m_RunningProcess != 0 && m_RunningProcess->GetPID() == pid) {
			LOG(L"Running proc is kill target, terminating...");

			//terminate it
			TerminateRunningProcess();
			return;
		}

		LOG(L"Looking for kill target in RDY");
		//find proc in ready queue
		Process* proc = m_ReadyProcesses.GetProcessWithID(pid);
		if (proc != 0) {
			LOG(L"Found it, killing...");

			//remove from ready
			m_ReadyProcesses.Remove(proc);

			//terminate
			TerminateProcess(proc);
		}
		else {
			LOG(L"Not found, ignoring sigkill...");
		}
	}

	void ProcessorFCFS::RequeueRunningProcess() {
		if (m_RunningProcess != 0) {
			m_ReadyProcesses.Add(m_RunningProcess);
		}

		Processor::RequeueRunningProcess();
	}
	
	void ProcessorFCFS::RegisterSigkillInfo(SigkillTimeInfo sigkill) {
		ms_Sigkills.Enqueue(sigkill);
	}
}
