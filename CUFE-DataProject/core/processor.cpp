#include "processor.h"
#include "scheduler.h"
#include "random_engine.h"
#include "processor_fcfs.h"

namespace core {
	Processor::Processor(ProcessorType type, Scheduler* scheduler) : m_Type(type), m_Scheduler(scheduler), m_ConcurrentTimer(0), 
		m_State(ProcessorState::IDLE), m_RunningProcess(0) {
		memset(m_StateTimers, 0, 3 * sizeof(int));
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

	void Processor::SetState(ProcessorState state) {
		m_State = state;
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

	void Processor::UpdateStateTimer() {
		if (m_State == ProcessorState::STOP) {
			m_StateTimers[(int)ProcessorState::STOP]++;
			return;
		}

		m_StateTimers[(int)(IsBusy() ? ProcessorState::BUSY : ProcessorState::IDLE)]++;
	}

	int Processor::GetStateTime(ProcessorState state) {
		return m_StateTimers[(int)state];
	}

	float Processor::GetProcessorLoad() {
		int trt = m_Scheduler->GetStatistics()->GetTotalTurnaroundDuration();
		int busyTime = GetStateTime(ProcessorState::BUSY);
		if (trt == 0) return busyTime > 0 ? 1.f : 0.f; //dont divide by zero

		return busyTime / (float)trt;
	}

	float Processor::GetProcessorUtilization() {
		int busyTime = GetStateTime(ProcessorState::BUSY);
		int idleTime = GetStateTime(ProcessorState::IDLE);
		float denominator = busyTime + idleTime;
		return denominator == 0.f ? 0.f : busyTime / (float)(busyTime + idleTime);
	}

	void Processor::ResetStateTimer(ProcessorState state) {
		m_StateTimers[(int)state] = 0;
	}

	void Processor::CheckOverheat() {
		int num = RandomEngine::GetInt(1, 1000);
		if (num <= OVERHEAT_PROB && m_Scheduler->CanProcessorOverheat(m_Type)) {
			//check if fcfs and has orphans
			if (m_Type == ProcessorType::FCFS) {
				if (((ProcessorFCFS*)this)->HasOrphans()) {
					return;
				}
			}

			//overheat !!
			SetState(ProcessorState::STOP);

			PUSHCOL(COL(BLACK, WHITE));
			LOG(L"OVERHEATING PROCESSOR");

			//start migrating all
			MigrateAllProcesses();

			LOG(L"OVERHEATING DONE");

			POPCOL();
		}
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

		case ProcessorState::STOP:
			return L"STOP";
		}

		return L"";
	}
}