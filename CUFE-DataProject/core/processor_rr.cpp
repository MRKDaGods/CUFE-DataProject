#include "processor_rr.h"
#include "scheduler.h"

namespace core {
	ProcessorRR::ProcessorRR(Scheduler* scheduler) : Processor(ProcessorType::RR, scheduler), m_ProcessStartTicks(0) {
	}

	void ProcessorRR::ScheduleAlgo() {
		//check for Time Slice
		if (m_RunningProcess != 0) {
			//check for migration of currently running process
			if (!TryMigrate(m_RunningProcess)) {
				int procTicks = m_RunningProcess->GetTicks();
				if (procTicks > m_ProcessStartTicks && procTicks % m_Scheduler->GetLoadFileInfo()->data.rr_timeslice == 0) {
					LOG(L"Process reached RR slice, requeuing...");

					//remove process
					RequeueRunningProcess();
				}
			}
		}

		//process mightve been migrated and therefore m_RunningProcess is null

		//get process from ready
		Process* proc = 0;

		do {
			//keep on picking a new process until migration doesnt occur
			if (m_RunningProcess == 0) {
				m_ReadyProcesses.Dequeue(&proc);
			}
		} while (TryMigrate(proc));

		if (proc != 0) {
			//run it
			RunProcess(proc);

			//set tick start
			m_ProcessStartTicks = proc->GetTicks();

			LOGF(L"RR new process, proc star ticks=%d", m_ProcessStartTicks);
		}
	}

	void ProcessorRR::QueueProcess(Process* proc) {
		//update timer
		Processor::QueueProcess(proc);

		//add to ready list
		m_ReadyProcesses.Enqueue(proc);
	}

	void ProcessorRR::Print(_STD wstringstream& stream) {
		Processor::Print(stream);

		//print ready queue
		stream << m_ReadyProcesses.GetLength() << L" RDY: ";

		//print procs
		m_ReadyProcesses.Print(stream);

		//terminator
		stream << L'\n';
	}

	void ProcessorRR::RequeueRunningProcess() {
		if (m_RunningProcess != 0) {
			m_ReadyProcesses.Enqueue(m_RunningProcess);
		}

		Processor::RequeueRunningProcess();
	}

	bool ProcessorRR::TryMigrate(Process*& proc) {
		if (proc == 0) return false;

		//check if proc is forked
		if (proc->IsForked()) return false;

		LoadFileInfo* fileInfo = m_Scheduler->GetLoadFileInfo();

		//check for RR processors
		if (fileInfo->data.num_processors_rr <= 0) return false;

		//check for rtf constraint
		if (proc->GetRemainingTime() >= fileInfo->data.rtf) return false;

		//start migration
		//remove proc
		if (m_RunningProcess == proc) {
			m_RunningProcess = 0;
		}

		m_Scheduler->MigrateProcess(proc, ProcessorType::SJF);

		//set null
		proc = 0;

		return true;
	}
}