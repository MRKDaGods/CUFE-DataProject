#include "process.h"

namespace core {
	Process::Process(int pid, int at, int ct, ProcessIOData* ioData, int ioDataSz) : m_PID(pid), m_ArrivalTime(at), m_CpuTime(ct), m_ResponseTime(0), m_TerminationTime(0), m_State(ProcessState::NEW) {
		//check for and enqueue io data
		if (ioData != 0 && ioDataSz > 0) {
			for (int i = 0; i < ioDataSz; i++) {
				m_IODataQueue.Enqueue(ioData[i]);

				//caller is responsible for deleting ioData
			}
		}
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
