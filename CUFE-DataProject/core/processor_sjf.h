#pragma once

#include "../common.h"
#include "processor.h"
#include "process.h"

namespace core {
	class ProcessorSJF : public Processor {
	private:
		_COLLECTION ProcessLinkedPriorityQueue m_ReadyProcesses;

	public:
		ProcessorSJF(Scheduler* scheduler);

		virtual void ScheduleAlgo() override;
		virtual void QueueProcess(Process* proc) override;
		virtual void Print(_STD wstringstream& stream) override;

		// Returns a steal handle for a process, if applicable
		virtual bool GetStealHandle(StealHandle* stealHandle) override;
	};
}