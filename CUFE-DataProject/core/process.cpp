#include "process.h"

namespace core {
	Process::Process(int pid, int ct) : m_PID(pid), m_CpuTime(ct), m_ArrivalTime(0), m_ResponseTime(0),
		m_TerminationTime(0), m_State(ProcessState::NEW), m_IODataQueue()
	{
	}

	void Process::SetArrivalTime(int at)
	{
		m_ArrivalTime = at;
	}

	int Process::GetTurnaroundDuration()
	{
		return m_TerminationTime - m_ArrivalTime;
	}

	int Process::GetWaitingTime()
	{
		return GetTurnaroundDuration() - m_CpuTime;
	}
}
