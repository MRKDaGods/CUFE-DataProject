#include "processor_sjf.h"

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
}