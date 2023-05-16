#pragma once

#include "../common.h"
#include "../collections/linked_list.h"
#include "processor.h"
#include "process.h"
#include "deserializer.h"

namespace core {
	class ProcessorFCFS : public Processor {
	private:
		_COLLECTION ProcessLinkedList m_ReadyProcesses;

		/// <summary>
		/// Queue of sigkills
		/// </summary>
		static _COLLECTION LinkedQueue<SigkillTimeInfo> ms_Sigkills;

		/// <summary>
		/// Processes a sigkill
		/// </summary>
		void ProcessSigkill(int pid);

	protected:
		// Attempt to migrate the process from this processor to another
		virtual bool TryMigrate(Process*& proc) override;

	public:
		ProcessorFCFS(Scheduler* scheduler);

		virtual void ScheduleAlgo() override;
		virtual void QueueProcess(Process* proc) override;
		virtual void Print(_STD wstringstream& stream) override;

		// Kills a process with the specified pid
		void KillProcess(int pid);

		/// <summary>
		/// Attempts to kill a random process in the RDY list
		/// </summary>
		void KillRandomProcess();

		/// <summary>
		/// Requeue currently running process
		/// </summary>
		virtual void RequeueRunningProcess() override;

		/// <summary>
		/// Queues a process sigkill
		/// </summary>
		static void RegisterSigkillInfo(SigkillTimeInfo sigkill);
	};
}