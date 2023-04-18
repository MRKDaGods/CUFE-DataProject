#pragma once

#include "states.h"
#include "process.h"

#include <sstream>

namespace core {
	class Scheduler;
	
	enum class ProcessorType {
		FCFS,
		SJF,
		RR
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
		void TerminateRunningProcess(Scheduler* scheduler);

		/// <summary>
		/// Blocks the currently running process
		/// </summary>
		void BlockRunningProcess(Scheduler* scheduler);

	public:
		Processor(ProcessorType type);

		/// <summary>
		/// The processor type
		/// </summary>
		ProcessorType GetProcessorType();

		/// <summary>
		/// Returns the concurrent timer value
		/// </summary>
		int GetConcurrentTimer();

		/// <summary>
		/// Returns the current processor state
		/// </summary>
		ProcessorState GetState();

		/// <summary>
		/// <para>Scheduling algorithm for the next process</para>
		/// <para>Pretty much the processor's update function</para>
		/// </summary>
		virtual void ScheduleAlgo(Scheduler* scheduler) abstract;

		/// <summary>
		/// Adds a process to the processor's RDY list
		/// </summary>
		virtual void QueueProcess(Process* proc);

		/// <summary>
		/// Prints processor data into stream
		/// </summary>
		virtual void Print(_STD wstringstream& stream);
	};

	/// <summary>
	/// Converts ProcessorType to a wide string
	/// </summary>
	_STD wstring ProcessorTypeToWString(ProcessorType& type);

	/// <summary>
	/// Converts ProcesserState to a wide string
	/// </summary>
	_STD wstring ProcessorStateToWString(ProcessorState& state);
}