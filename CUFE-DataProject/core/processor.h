#pragma once

#include "states.h"
#include "process.h"

namespace core {
	class Processor {
	protected:
		/// <summary>
		/// Current processor state
		/// </summary>
		ProcessorState m_State;

	public:
		Processor();

		/// <summary>
		/// Returns the current processor state
		/// </summary>
		ProcessorState GetState();

		/// <summary>
		/// Scheduling algorithm for the next process
		/// </summary>
		virtual void ScheduleAlgo(Process* process) abstract;
	};
}