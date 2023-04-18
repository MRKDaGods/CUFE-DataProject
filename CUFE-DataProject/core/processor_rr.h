#pragma once

#include "../common.h"
#include "../collections/linked_list.h"
#include "processor.h"
#include "process.h"

namespace core {
	class ProcessorRR : public Processor {
	private:
		_COLLECTION ProcessLinkedQueue m_ReadyProcesses;

	public:
		ProcessorRR(Scheduler* scheduler);

		virtual void ScheduleAlgo() override;
		virtual void QueueProcess(Process* proc) override;
		virtual void Print(_STD wstringstream& stream) override;
	};
}