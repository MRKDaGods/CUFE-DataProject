#include "processor.h"
#include "scheduler.h"

namespace core {
	Processor::Processor(ProcessorType type, Scheduler* scheduler) : m_Type(type), m_Scheduler(scheduler), m_ConcurrentTimer(0), m_State(ProcessorState::IDLE), m_RunningProcess(0) {
	}

	ProcessorType Processor::GetProcessorType() {
		return m_Type;
	}

	int Processor::GetConcurrentTimer(bool withRunning) {
		if (withRunning || m_RunningProcess == 0) return m_ConcurrentTimer;

		return m_ConcurrentTimer - m_RunningProcess->GetRemainingTime();
	}

	ProcessorState Processor::GetState() {
		return m_State;
	}

	Process* Processor::GetRunningProcess() {
		return m_RunningProcess;
	}

	void Processor::DecrementTimer(Process* proc) {
		m_ConcurrentTimer -= proc != 0 ? proc->GetRemainingTime() : 1;
	}

	void Processor::TerminateProcess(Process* proc) {
		if (proc == 0) return;

		//decrement timer
		DecrementTimer(proc);

		//update state to TRM
		proc->SetState(ProcessState::TRM);

		//we are not the owner anymore
		proc->SetOwner(0);

		//move process to trm list
		m_Scheduler->NotifyProcessTerminated(proc);
	}

	bool Processor::TryMigrate(Process*& proc) {
		return false;
	}

	void Processor::RunProcess(Process* proc) {
		//update process state to RUN
		proc->SetState(ProcessState::RUN);

		//update currently running process
		m_RunningProcess = proc;

		//process is now busy
		m_State = ProcessorState::BUSY;

		//set owner incase
		proc->SetOwner(this);
	}

	void Processor::TerminateRunningProcess() {
		if (m_RunningProcess == 0) return;

		//terminate
		TerminateProcess(m_RunningProcess);

		//no running procs now
		m_RunningProcess = 0;

		//update state to idle
		m_State = ProcessorState::IDLE;
	}

	void Processor::BlockRunningProcess() {
		if (m_RunningProcess == 0) return;

		//decrement timer
		DecrementTimer(m_RunningProcess);

		//update state to BLK
		m_RunningProcess->SetState(ProcessState::BLK);

		//we are not the owner anymore
		m_RunningProcess->SetOwner(0);

		//move process to trm list
		m_Scheduler->NotifyProcessBlocked(m_RunningProcess);

		//no running procs now
		m_RunningProcess = 0;

		//update state to idle
		m_State = ProcessorState::IDLE;
	}

	void Processor::RequeueRunningProcess() {
		if (m_RunningProcess == 0) return;

		//update state to RDY
		m_RunningProcess->SetState(ProcessState::RDY);

		//no running procs now
		m_RunningProcess = 0;

		//update state to idle
		m_State = ProcessorState::IDLE;
	}

	void Processor::QueueProcess(Process* proc) {
		//increment timer
		m_ConcurrentTimer += proc->GetRemainingTime();

		//update state to RDY
		proc->SetState(ProcessState::RDY);

		//set owner
		proc->SetOwner(this);
	}

	void Processor::Print(_STD wstringstream& stream) {
		stream << L"[" << ProcessorStateToWString(m_State) 
			<< L"] Processor (" 
			<< ProcessorTypeToWString(m_Type) 
			<< L") [" 
			<< GetConcurrentTimer() 
			<< L"]: ";
	}

	_STD wstring ProcessorTypeToWString(ProcessorType type) {
		switch (type) {
		case ProcessorType::FCFS:
			return L"FCFS";

		case ProcessorType::SJF:
			return L"SJF";

		case ProcessorType::RR:
			return L"RR";

		case ProcessorType::EDF:
			return L"EDF";
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