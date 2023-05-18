#pragma once

#include "../common.h"
#include "processor.h"
#include "process.h"

namespace core {
	class ProcessorSJF : public Processor {
	private:
		_COLLECTION ProcessLinkedPriorityQueue<_COLLECTION ProcessRemainingTimePriority> m_ReadyProcesses;

	protected:
		/// Migrates all the processes to other processors
		virtual void MigrateAllProcesses() override;

		virtual bool IsBusy() override;

	public:
		ProcessorSJF(Scheduler* scheduler);

		virtual void ScheduleAlgo() override;
		virtual void QueueProcess(Process* proc) override;
		virtual void Print(_STD wstringstream& stream) override;

		// Returns a steal handle for a process, if applicable
		virtual bool GetStealHandle(StealHandle* stealHandle) override;
	};
}