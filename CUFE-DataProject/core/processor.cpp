#include "processor.h"
#include "scheduler.h"

namespace core {
	Processor::Processor(ProcessorType type) : m_Type(type), m_ConcurrentTimer(0), m_State(ProcessorState::IDLE), m_RunningProcess(0) {
	}

	ProcessorType Processor::GetProcessorType() {
		return m_Type;
	}

	int Processor::GetConcurrentTimer() {
		return m_ConcurrentTimer;
	}

	ProcessorState Processor::GetState() {
		return m_State;
	}

	void Processor::DecrementTimer(Process* proc) {
		m_ConcurrentTimer -= proc != 0 ? (proc->GetCPUTime() - proc->GetTicks()) : 1;
	}

	void Processor::RunProcess(Process* proc) {
		//update process state to RUN
		proc->SetState(ProcessState::RUN);

		//update currently running process
		m_RunningProcess = proc;

		//process is now busy
		m_State = ProcessorState::BUSY;
	}

	void Processor::TerminateRunningProcess(Scheduler* scheduler) {
		if (m_RunningProcess == 0) return;

		//decrement timer
		DecrementTimer(m_RunningProcess);

		//update state to TRM
		m_RunningProcess->SetState(ProcessState::TRM);

		//move process to trm list
		scheduler->NotifyProcessTerminated(m_RunningProcess);

		//no running procs now
		m_RunningProcess = 0;

		//update state to idle
		m_State = ProcessorState::IDLE;
	}

	void Processor::BlockRunningProcess(Scheduler* scheduler) {
		if (m_RunningProcess == 0) return;

		//decrement timer
		DecrementTimer(m_RunningProcess);

		//update state to BLK
		m_RunningProcess->SetState(ProcessState::BLK);

		//move process to trm list
		scheduler->NotifyProcessBlocked(m_RunningProcess);

		//no running procs now
		m_RunningProcess = 0;

		//update state to idle
		m_State = ProcessorState::IDLE;
	}

	void Processor::QueueProcess(Process* proc) {
		//increment timer
		m_ConcurrentTimer += proc->GetCPUTime();

		//update state to RDY
		proc->SetState(ProcessState::RDY);
	}

	void Processor::Print(_STD wstringstream& stream) {
		stream << L"[" << ProcessorStateToWString(m_State) << L"] Processor (" << ProcessorTypeToWString(m_Type) << L") [" << m_ConcurrentTimer << L"]: ";
	}

	_STD wstring ProcessorTypeToWString(ProcessorType& type) {
		switch (type) {
		case ProcessorType::FCFS:
			return L"FCFS";

		case ProcessorType::SJF:
			return L"SJF";

		case ProcessorType::RR:
			return L"RR";
		}

		return L"";
	}

	_STD wstring ProcessorStateToWString(ProcessorState& state) {
		switch (state) {
		case ProcessorState::BUSY:
			return L"BUSY";

		case ProcessorState::IDLE:
			return L"IDLE";
		}

		return L"";
	}
}