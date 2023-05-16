#pragma once

#include "states.h"
#include "process.h"

#include <sstream>
#include <functional>

namespace core {
	class Scheduler;
	
	enum class ProcessorType {
		None,
		FCFS,
		SJF,
		RR
	};

	// Simple handle containing a process to be stolen, and a steal delegate
	struct StealHandle {
		// Steal applicable process
		Process* process;

		// Steal executor
		_STD function<void()> execute;
	};

	class Processor {
	private:
		/// <summary>
		/// The processor type
		/// </summary>
		ProcessorType m_Type;

		/// <summary>
		/// Timer to keep track of how long processes are going to run for
		/// </summary>
		int m_ConcurrentTimer;
	
	protected:
		/// <summary>
		/// Current processor state
		/// </summary>
		ProcessorState m_State;

		/// <summary>
		/// Currently running process
		/// </summary>
		Process* m_RunningProcess;

		/// <summary>
		/// The scheduler
		/// </summary>
		Scheduler* m_Scheduler;

		/// <summary>
		/// Terminates a process (does not alter the processor state)
		/// </summary>
		void TerminateProcess(Process* proc);

		// Attempt to migrate the process from this processor to another
		virtual bool TryMigrate(Process*& proc);

	public:
		Processor(ProcessorType type, Scheduler* scheduler);

		/// <summary>
		/// The processor type
		/// </summary>
		ProcessorType GetProcessorType();

		/// <summary>
		/// Returns the concurrent timer value
		/// </summary>
		int GetConcurrentTimer(bool withRunning = true);

		/// <summary>
		/// Returns the current processor state
		/// </summary>
		ProcessorState GetState();

		/// <summary>
		/// Returns the currently running process
		/// </summary>
		Process* GetRunningProcess();

		/// <summary>
		/// <para>Scheduling algorithm for the next process</para>
		/// <para>Pretty much the processor's update function</para>
		/// </summary>
		virtual void ScheduleAlgo() abstract;

		/// <summary>
		/// Adds a process to the processor's RDY list
		/// </summary>
		virtual void QueueProcess(Process* proc);

		/// <summary>
		/// Removes the time occupied by a process, decrements by 1 if no process supplied
		/// </summary>
		void DecrementTimer(Process* proc = 0);

		/// <summary>
		/// Runs a process
		/// </summary>
		void RunProcess(Process* proc);

		/// <summary>
		/// Terminates the currently running process
		/// </summary>
		void TerminateRunningProcess();

		/// <summary>
		/// Blocks the currently running process
		/// </summary>
		void BlockRunningProcess();

		/// <summary>
		/// Puts back the currently running process in the RDY queue
		/// </summary>
		virtual void RequeueRunningProcess();

		// Returns a steal handle for a process, if applicable
		virtual bool GetStealHandle(StealHandle* stealHandle) abstract;

		/// <summary>
		/// Prints processor data into stream
		/// </summary>
		virtual void Print(_STD wstringstream& stream);
	};

	/// <summary>
	/// Converts ProcessorType to a wide string
	/// </summary>
	_STD wstring ProcessorTypeToWString(ProcessorType type);

	/// <summary>
	/// Converts ProcesserState to a wide string
	/// </summary>
	_STD wstring ProcessorStateToWString(ProcessorState& state);
}