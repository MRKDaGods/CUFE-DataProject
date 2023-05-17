#pragma once

#include "../common.h"
#include "../collections/linked_list.h"
#include "../collections/linked_queue.h"
#include "../collections/linked_priority_queue.h"
#include "../collections/binary_tree.h"
#include "states.h"

#include <sstream>
#include <functional>

#define PROC_BT_NODE _COLLECTION BinaryTreeNode<Process*>

namespace core {
	class Process;
	class Processor;

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

	struct ForkingData {
		// Fork tree
		_COLLECTION BinaryTree<Process*> fork_tree;

		// Our foreign node address
		PROC_BT_NODE** forgein_node;

		// Iterates the forked children
		void Iterate(_STD function<void(PROC_BT_NODE*)> iterator);
	};

	class Process {
	private:
		// Process unique id
		int m_PID;

		int m_ArrivalTime; // AT
		int m_ResponseTime; // RT
		int m_CpuTime; // CT
		int m_TerminationTime; // TT
		
		// Process absolute deadline (EDF)
		int m_Deadline;

		/// <summary>
		/// Realtime execution timer
		/// </summary>
		int m_Ticks;

		// The processor currently owning this process
		// (Process is in RDY/RUN) state
		Processor* m_Owner;

		// Current process state
		ProcessState m_State;

		// IO data qeueue
		_COLLECTION LinkedQueue<ProcessIOData> m_IODataQueue;

		// Forking related info
		ForkingData m_ForkingData;

		friend _STD wstringstream& operator<<(_STD wstringstream& stream, Process* proc);

	public:
		Process(int pid, int at, int ct, int deadline, ProcessIOData* ioData = 0, int ioDataSz = 0);

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

		// The process absolute deadline
		int GetDeadline();

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

		// The owning processor
		Processor* GetOwner();

		// Sets the owning processor
		void SetOwner(Processor* processor);

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
		bool HasIOEvent();

		/// <summary>
		/// Does the process have any IO event?
		/// </summary>
		bool HasAnyIOEvent();

		/// <summary>
		/// Returns the IO data that is to be handled next, and pops it from the queue
		/// </summary>
		ProcessIOData GetIOData();

		/// <summary>
		/// Returns the time left for the process to run
		/// </summary>
		int GetRemainingTime();

		/// <summary>
		/// The process forking data
		/// </summary>
		ForkingData* GetForkingData();

		// Can the process fork?
		bool CanFork();

		// Is the process a forked one?
		bool IsForked();
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

	class ProcessLinkedQueue : public LinkedQueue<_CORE Process*> {
	public:
		void Print(_STD wstringstream& stream);
	};

	// Priority based on smaller RemainingTime
	struct ProcessRemainingTimePriority {
		bool operator()(_CORE Process* p1, _CORE Process* p2) {
			return p1->GetRemainingTime() < p2->GetRemainingTime();
		}
	};

	// Priority based on earlier deadline
	struct ProcessDeadlinePriority {
		bool operator()(_CORE Process* p1, _CORE Process* p2) {
			return p1->GetDeadline() < p2->GetDeadline();
		}
	};

	template<typename Comparer>
	class ProcessLinkedPriorityQueue : public LinkedPriorityQueue<_CORE Process*, Comparer> {
	public:
		void Print(_STD wstringstream& stream) {
			auto list = &(LinkedPriorityQueue<_CORE Process*, Comparer>::m_LinkedList);
			if (list->GetHead() == 0) return;

			for (LinkedListNode<_CORE Process*>* node = list->GetHead(); node; node = node->next) {
				stream << node->value << L", ";
			}
		}
	};
}