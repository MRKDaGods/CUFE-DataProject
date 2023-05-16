#pragma once

#include "../common.h"
#include "../collections/linked_list.h"
#include "processor.h"
#include "process.h"

namespace core {
	class ProcessorRR : public Processor {
	private:
		/// <summary>
		/// RR ready queue
		/// </summary>
		_COLLECTION ProcessLinkedQueue m_ReadyProcesses;

		/// <summary>
		/// Number of ticks at the time of running the process
		/// </summary>
		int m_ProcessStartTicks;

	protected:
		// Attempt to migrate the process from this processor to another
		virtual bool TryMigrate(Process*& proc) override;

	public:
		ProcessorRR(Scheduler* scheduler);

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