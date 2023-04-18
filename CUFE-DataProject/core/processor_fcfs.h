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
		ProcessorFCFS(Scheduler* scheduler);

		virtual void ScheduleAlgo() override;
		virtual void QueueProcess(Process* proc) override;
		virtual void Print(_STD wstringstream& stream) override;

		/// <summary>
		/// Attempts to kill a random process in the RDY list
		/// </summary>
		void KillRandomProcess();

		/// <summary>
		/// Processes a sigkill
		/// </summary>
		void ProcessSigkill(int pid);
	};
}