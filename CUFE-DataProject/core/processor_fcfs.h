#pragma once

#include "../common.h"
#include "../collections/linked_list.h"
#include "processor.h"
#include "process.h"

namespace core {
	class ProcessorFCFS : public Processor {
	private:
		_COLLECTION ProcessLinkedList m_ReadyProcesses;

	public:
		ProcessorFCFS();

		virtual void ScheduleAlgo(Scheduler* scheduler) override;
		virtual void QueueProcess(Process* proc) override;
		virtual void Print(_STD wstringstream& stream) override;
	};
}