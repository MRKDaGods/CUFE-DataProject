#pragma once

#include "../common.h"
#include "../collections/linked_list.h"
#include "../collections/linked_queue.h"
#include "../collections/linked_priority_queue.h"
#include "states.h"

#include <sstream>

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
		/// Realtime execution timer
		/// </summary>
		int m_Ticks;

		/// <summary>
		/// Current process state
		/// </summary>
		ProcessState m_State;

		/// <summary>
		/// IO data qeueue
		/// </summary>
		_COLLECTION LinkedQueue<ProcessIOData> m_IODataQueue;

		/// <summary>
		/// Process child
		/// </summary>
		Process* m_ChildProcess;

		friend _STD wstringstream& operator<<(_STD wstringstream& stream, Process* proc);

	public:
		Process(int pid, int at, int ct, ProcessIOData* ioData = 0, int ioDataSz = 0);

		/// <summary>
		/// Returns the process id
		/// </summary>
		int GetPID();

		/// <summary>
		/// (AT) Returns the arrival time
		/// </summary>
		int GetArrivalTime();

		/// <summary>
		/// (CT) Returns the CpuTime
		/// </summary>
		int GetCPUTime();

		/// <summary>
		/// (TRT) Total time a proc spends in the system from its arrival to its termination
		/// </summary>
		int GetTurnaroundDuration();

		/// <summary>
		/// (WT) Total time a proc spends in the system not being executed by the CPU
		/// </summary>
		int GetWaitingTime();

		/// <summary>
		/// Returns the total process ticks
		/// </summary>
		int GetTicks();

		/// <summary>
		/// Returns the current process state
		/// </summary>
		ProcessState GetState();

		/// <summary>
		/// Updates the process state
		/// </summary>
		void SetState(ProcessState state);

		/// <summary>
		/// Increments the ticks
		/// </summary>
		void Tick();

		/// <summary>
		/// Has the process finished executing?
		/// </summary>
		bool IsDone();

		/// <summary>
		/// Does the process have an IO event now?
		/// </summary>
		bool HasIOEvent(int currentTs);

		/// <summary>
		/// Does the process have any IO event?
		/// </summary>
		bool HasAnyIOEvent();

		/// <summary>
		/// Returns the IO data that is to be handled next, and pops it from the queue
		/// </summary>
		ProcessIOData GetIOData();
	};
}

namespace collections {
	class ProcessLinkedList : public _COLLECTION LinkedList<_CORE Process*> {
	public:
		void Print(_STD wstringstream& stream);

		/// <summary>
		/// Returns the process with id=pid (optimized)
		/// </summary>
		_CORE Process* GetProcessWithID(int pid);
	};

	class ProcessLinkedQueue : public _COLLECTION LinkedQueue<_CORE Process*> {
	public:
		void Print(_STD wstringstream& stream);
	};

	struct ProcessPriority {
		bool operator()(_CORE Process* p1, _CORE Process* p2) {
			return p1->GetCPUTime() < p2->GetCPUTime();
		}
	};

	class ProcessLinkedPriorityQueue : public _COLLECTION LinkedPriorityQueue<_CORE Process*, ProcessPriority> {
	public:
		void Print(_STD wstringstream& stream);
	};
}