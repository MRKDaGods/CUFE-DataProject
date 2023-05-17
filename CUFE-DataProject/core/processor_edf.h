#pragma once

#include "../common.h"
#include "processor.h"
#include "process.h"

namespace core {
	class ProcessorEDF : public Processor {
	private:
		_COLLECTION ProcessLinkedPriorityQueue<_COLLECTION ProcessDeadlinePriority> m_ReadyProcesses;

	public:
		ProcessorEDF(Scheduler* scheduler);

		virtual void ScheduleAlgo() override;
		virtual void QueueProcess(Process* proc) override;
		virtual void Print(_STD wstringstream& stream) override;

		/// <summary>
		/// Requeue currently running process
		/// </summary>
		virtual void RequeueRunningProcess() override;

		// Returns a steal handle for a process, if applicable
		virtual bool GetStealHandle(StealHandle* stealHandle) override;
	};
}