#include "process.h"

#include <concurrent_priority_queue.h>

namespace core {
	Process::Process(int pid, int at, int ct, ProcessIOData* ioData, int ioDataSz) : m_PID(pid), m_ArrivalTime(at), m_CpuTime(ct), m_ResponseTime(0), m_TerminationTime(0),
		m_Ticks(0), m_State(ProcessState::NEW) {
		//check for and enqueue io data
		if (ioData != 0 && ioDataSz > 0) {
			for (int i = 0; i < ioDataSz; i++) {
				m_IODataQueue.Enqueue(ioData[i]);

				//caller is responsible for deleting ioData
			}
		}
	}

	int Process::GetPID() {
		return m_PID;
	}

	int Process::GetArrivalTime() {
		return m_ArrivalTime;
	}

	int Process::GetCPUTime() {
		return m_CpuTime;
	}

	int Process::GetTurnaroundDuration() {
		return m_TerminationTime - m_ArrivalTime;
	}

	int Process::GetWaitingTime() {
		return GetTurnaroundDuration() - m_CpuTime;
	}

	int Process::GetTicks() {
		return m_Ticks;
	}

	ProcessState Process::GetState() {
		return m_State;
	}

	void Process::SetState(ProcessState state) {
		m_State = state;
	}

	void Process::Tick() {
		m_Ticks++;
	}

	bool Process::IsDone() {
		return m_Ticks == m_CpuTime;
	}

	bool Process::HasIOEvent(int currentTs) {
		ProcessIOData ioData;
		return m_IODataQueue.Peek(&ioData) && ioData.request_time == currentTs - m_ArrivalTime;
	}

	bool Process::HasAnyIOEvent() {
		return !m_IODataQueue.IsEmpty();
	}

	ProcessIOData Process::GetIOData() {
		ProcessIOData data;
		m_IODataQueue.Dequeue(&data);

		return data;
	}

	_STD wstringstream& operator<<(_STD wstringstream& stream, Process* proc) {
		stream << proc->m_PID;
		return stream;
	}
}

namespace collections {
	void ProcessLinkedList::Print(_STD wstringstream& stream) {
		if (m_Head == 0) return;

		for (LinkedListNode<_CORE Process*>* node = m_Head; node; node = node->next) {
			stream << node->value << L", ";
		}
	}

	_CORE Process* ProcessLinkedList::GetProcessWithID(int pid) {
		if (m_Head == 0) return 0;

		//quick check
		if (m_Head->value->GetPID() == pid) return m_Head->value;
		if (m_Head->value->GetPID() == pid) return m_Tail->value;

		//not head, not tail, start traversal
		LinkedListNode<_CORE Process*>* node = m_Head;
		while (node = node->next) {
			if (node->value->GetPID() == pid) {
				return node->value;
			}
		}

		return 0;
	}

	void ProcessLinkedQueue::Print(_STD wstringstream& stream) {
		if (m_LinkedList.GetHead() == 0) return;

		for (LinkedListNode<_CORE Process*>* node = m_LinkedList.GetHead(); node; node = node->next) {
			stream << node->value << L", ";
		}
	}

	void ProcessLinkedPriorityQueue::Print(_STD wstringstream& stream) {
		if (m_LinkedList.GetHead() == 0) return;

		for (LinkedListNode<_CORE Process*>* node = m_LinkedList.GetHead(); node; node = node->next) {
			stream << node->value << L", ";
		}
	}
}
