#include "processor_rr.h"
#include "scheduler.h"

namespace core {
	ProcessorRR::ProcessorRR(Scheduler* scheduler) : Processor(ProcessorType::RR, scheduler), m_ProcessStartTicks(0) {
	}

	void ProcessorRR::ScheduleAlgo() {
		//check for Time Slice
		if (m_RunningProcess != 0) {
			int procTicks = m_RunningProcess->GetTicks();
			if (procTicks > m_ProcessStartTicks && procTicks % m_Scheduler->GetLoadFileInfo()->rr_timeslice == 0) {
				LOG(L"Process reached RR slice, requeuing...");

				//remove process
				RequeueRunningProcess();
			}
		}

		//get process from ready
		Process* proc = 0;
		if (m_RunningProcess == 0 && m_ReadyProcesses.Dequeue(&proc)) {
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
}