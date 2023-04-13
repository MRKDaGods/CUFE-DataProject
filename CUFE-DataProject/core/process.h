#pragma once

#include "../common.h"
#include "states.h"
#include "../collections/linked_queue.h"

namespace core {
	/// <summary>
	/// IO data pair (IO_R, IO_D)
	/// </summary>
	struct ProcessIOData {
		/// <summary>
		/// Execution interval after which the process requests for IO resources
		/// </summary>
		int request_time;

		/// <summary>
		/// IO task duration
		/// </summary>
		int duration;

		bool operator==(ProcessIOData& other) {
			return request_time == other.request_time && duration == other.duration;
		}
	};

	class Process {
	private:
		/// <summary>
		/// Process unique id
		/// </summary>
		int m_PID;

		int m_ArrivalTime; // AT
		int m_ResponseTime; // RT
		int m_CpuTime; // CT
		int m_TerminationTime; // TT

		/// <summary>
		/// Current process state
		/// </summary>
		ProcessState m_State;

		/// <summary>
		/// IO data qeueue
		/// </summary>
		_COLLECTION LinkedQueue<ProcessIOData> m_IODataQueue;

	public:
		Process(int pid, int ct);

		/// <summary>
		/// Sets the process arrival time
		/// </summary>
		void SetArrivalTime(int at);

		/// <summary>
		/// (TRT) Total time a proc spends in the system from its arrival to its termination
		/// </summary>
		int GetTurnaroundDuration();

		/// <summary>
		/// (WT) Total time a proc spends in the system not being executed by the CPU
		/// </summary>
		int GetWaitingTime();
	};
}