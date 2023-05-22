#include "processor_fcfs.h"
#include "scheduler.h"
#include "random_engine.h"

namespace core {
	_COLLECTION LinkedQueue<SigkillTimeInfo> ProcessorFCFS::ms_Sigkills;

	ProcessorFCFS::ProcessorFCFS(Scheduler* scheduler) : Processor(ProcessorType::FCFS, scheduler) {
	}
	
	void ProcessorFCFS::ScheduleAlgo() {
		if (m_RunningProcess != 0) {
			//check for migration of currently running process
			if (!TryMigrate(m_RunningProcess)) {
				//check for forking
				if (m_RunningProcess->CanFork()) {
					//generate probability
					int num = RandomEngine::GetInt(1, 100);
					if (num <= m_Scheduler->GetLoadFileInfo()->data.fork_prob) {
						//fork !!!
						m_Scheduler->ForkProcess(m_RunningProcess);
					}
				}
			}
		}

		//process mightve been migrated and therefore m_RunningProcess is null

		//get process from ready
		Process* proc = 0;

		do {
			//keep on picking a new process until migration doesnt occur
			if (m_RunningProcess == 0 && m_ReadyProcesses.GetLength() > 0) {
				//running proc should be head O(1)
				proc = *m_ReadyProcesses[0];

				//remove from head O(1)
				m_ReadyProcesses.Remove(proc);
			}
		} while (TryMigrate(proc));

		if (proc != 0) {
			RunProcess(proc);
		}

		LOG(L"Looking for sigkill");

		//process sigkill for FCFS -- many sigkills can occur at the same timestep
		SigkillTimeInfo sigkill;
		while (ms_Sigkills.Peek(&sigkill) && sigkill.time == m_Scheduler->GetSimulationInfo()->GetTimestep()) {
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

	void ProcessorFCFS::KillProcess(int pid) {
		LOGF(L"Killing process with pid=%d", pid);

		//check if it's the running process
		if (m_RunningProcess != 0 && m_RunningProcess->GetPID() == pid) {
			//yep it is
			TerminateRunningProcess();

			//increment statistic
			m_Scheduler->GetStatistics()->AddStatistic(StatisticType::Kill);
			return;
		}

		//search for it in RDY
		Process* proc = m_ReadyProcesses.GetProcessWithID(pid);
		if (proc != 0) {
			//remove from ready
			m_ReadyProcesses.Remove(proc);

			//terminate
			TerminateProcess(proc);

			//increment statistic
			m_Scheduler->GetStatistics()->AddStatistic(StatisticType::Kill);
		}
	}

	void ProcessorFCFS::KillRandomProcess() {
		LOG(L"Attempting to kill a random process");

		if (m_ReadyProcesses.GetLength() == 0) {
			LOG(L"RDY empty, cant kill");
			return;
		}

		int pid = RandomEngine::GetInt(1, m_Scheduler->GetLoadFileInfo()->data.proc_count);

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

			m_Scheduler->GetStatistics()->AddStatistic(StatisticType::Kill);
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

			m_Scheduler->GetStatistics()->AddStatistic(StatisticType::Kill);
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

	bool ProcessorFCFS::GetStealHandle(StealHandle* stealHandle) {
		if (stealHandle == 0) return false;

		//check for any RDY process
		if (m_ReadyProcesses.GetLength() == 0) return false;

		Process* proc = *m_ReadyProcesses[0];
		if (proc->IsForked()) return false; //no forked processes are applicable

		//process is applicable for stealing
		*stealHandle = {
			proc,
			[&]() -> void {
				//update timer
				DecrementTimer(proc);

				//remove from head of list
				m_ReadyProcesses.Remove(proc);
			}
		};

		return true;
	}

	bool ProcessorFCFS::HasOrphans() {
		if (m_RunningProcess && m_RunningProcess->IsForked()) return true;

		for (int i = 0; i < m_ReadyProcesses.GetLength(); i++) {
			Process* proc = *m_ReadyProcesses[i];
			if (proc->IsForked()) return true;
		}

		return false;
	}
	
	void ProcessorFCFS::RegisterSigkillInfo(SigkillTimeInfo sigkill) {
		ms_Sigkills.Enqueue(sigkill);
	}

	bool ProcessorFCFS::TryMigrate(Process*& proc) {
		if (proc == 0) return false;
		
		//check if proc is forked
		if (proc->IsForked()) return false;

		LoadFileInfo* fileInfo = m_Scheduler->GetLoadFileInfo();
		
		//check for RR processors
		if (fileInfo->data.num_processors_rr <= 0) return false;

		//check for maxw constraint
		int waitingTime = m_Scheduler->GetSimulationInfo()->GetTimestep() - proc->GetArrivalTime() - proc->GetTicks();
		if (waitingTime <= fileInfo->data.maxw) return false;

		//check for overheat
		if (m_Scheduler->GetNumberOfActiveProcessors(ProcessorType::RR) == 0) return false;

		//start migration

		//decrement timer
		DecrementTimer(proc);

		//set idle for now
		m_State = ProcessorState::IDLE;

		m_Scheduler->MigrateProcess(proc, ProcessorType::RR);

		//set null
		//remove proc
		if (m_RunningProcess == proc) {
			m_RunningProcess = 0;
		}

		proc = 0;

		return true;
	}

	void ProcessorFCFS::MigrateAllProcesses() {
		if (m_RunningProcess != 0) {
			DecrementTimer(m_RunningProcess);
			m_Scheduler->Schedule(m_RunningProcess, ProcessorType::None, this);

			m_RunningProcess = 0;
		}

		while (m_ReadyProcesses.GetLength() > 0) {
			Process* proc = *m_ReadyProcesses[0];
			
			DecrementTimer(proc);
			m_ReadyProcesses.Remove(proc);
			m_Scheduler->Schedule(proc, ProcessorType::None, this);
		}
	}

	bool ProcessorFCFS::IsBusy() {
		return m_RunningProcess || m_ReadyProcesses.GetLength() > 0;
	}
}
