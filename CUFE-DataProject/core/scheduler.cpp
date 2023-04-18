#include "scheduler.h"
#include "deserializer.h"
#include "processor_fcfs.h"
#include "processor_sjf.h"
#include "processor_rr.h"
#include "random_engine.h"

namespace core {
	Scheduler::Scheduler() : m_View(this, &m_UI) {
		//initialize ui controller
		m_UI.Initialize(APP_NAME, _UTIL Vector2(APP_SIZE_WIDTH, APP_SIZE_HEIGHT), _STD bind(&SchedulerView::UICallback, &m_View));

		//zero out load file info
		memset(&m_LoadFileInfo, 0, sizeof(LoadFileInfo));

		//zero out io mutex
		memset(&m_IOMutex, 0, sizeof(IOMutex));
	}
	
	Scheduler::~Scheduler() {
		//the underlying implementation of the [] operator iterates about the list
		//till it reaches the provided index, so a for loop would be inefficient
		//in order to prevent unnessecary iterations, we will keep removing the very first element
		//at index 0 aka keep removing the head

		//de alloc processors
		while (!m_Processors.IsEmpty()) {
			Processor* processor = *m_Processors[0];
			m_Processors.Remove(processor);

			delete processor;
		}
	}

	Processor* Scheduler::GetProcessorWithShortestQueue() {
		Processor* proc = *m_Processors[0];
		int val = proc->GetConcurrentTimer();

		for (int i = 1; i < m_Processors.GetLength(); i++) {
			Processor* cur = *m_Processors[i];
			if (cur->GetConcurrentTimer() < val) {
				proc = cur;
				val = cur->GetConcurrentTimer();
			}
		}

		return proc;
	}

	void Scheduler::UpdateIO() {
		//handle current execution
		if (m_IOMutex.owner != 0) {
			//decrement duration
			m_IOMutex.io_data.duration--;

			//we have finished
			if (m_IOMutex.io_data.duration == 0) {
				//process should be scheduled again
				Schedule(m_IOMutex.owner);

				//clean mutex
				memset(&m_IOMutex, 0, sizeof(IOMutex));
			}
		}

		//check for another IO operation
		if (m_IOMutex.owner == 0) {
			//find potential owner in BLK list
			if (m_BlockedProcesses.Dequeue(&m_IOMutex.owner)) {
				m_IOMutex.io_data = m_IOMutex.owner->GetIOData();
			}
		}
	}

	void Scheduler::UpdateProcessor(Processor* processor) {
		//currently running process is not null, check for completion
		Process* runningProc = processor->GetRunningProcess();
		if (runningProc != 0) {
			runningProc->Tick();

			//decrement timer by 1 tick
			processor->DecrementTimer();

			//check if proc has finished executing
			if (runningProc->IsDone()) {
				processor->TerminateRunningProcess();
			}
			else if (runningProc->HasIOEvent(m_SimulationInfo.GetTimestep())) { //check for IO
				//block process
				processor->BlockRunningProcess();
			}
		}

		//get running process again incase of change
		//run schedule algo incase of process null
		if (processor->GetRunningProcess() == 0) {
			processor->ScheduleAlgo();
		}

		//kill random proc in RDY
		if (processor->GetProcessorType() == ProcessorType::FCFS) {
			ProcessorFCFS* fcfs = (ProcessorFCFS*)processor;
			fcfs->KillRandomProcess();

			//process sigkill for FCFS
			SigkillTimeInfo sigkill;
			if (m_Sigkills.Peek(&sigkill) && sigkill.time == m_SimulationInfo.GetTimestep()) {
				//dequeue sigkill
				m_Sigkills.Dequeue();

				//process it
				fcfs->ProcessSigkill(sigkill.proc_pid);
			}
		}
	}

	LoadFileInfo* Scheduler::GetLoadFileInfo() {
		return &m_LoadFileInfo;
	}

	SimulationInfo* Scheduler::GetSimulationInfo() {
		return &m_SimulationInfo;
	}

	void Scheduler::Update() {
		//get proc at current timestep
		int ts = m_SimulationInfo.GetTimestep();
		Process* proc = 0;
		while (m_NewProcesses.Peek(&proc) && proc->GetArrivalTime() == ts) {
			//dequeue the proc
			m_NewProcesses.Dequeue();

			//schedule it
			Schedule(proc);
		}

		//update processors
		for (int i = 0; i < m_Processors.GetLength(); i++) {
			Processor* processor = *m_Processors[i];
			UpdateProcessor(processor);

			Process* proc = processor->GetRunningProcess();
			if (proc != 0) {
				int num = RandomEngine::GetInt(1, 100);
				if (num >= 1 && num <= 15) {
					//move to BLK
					//proc must have io data
					if (proc->HasAnyIOEvent()) {
						processor->BlockRunningProcess();
					}
				}
				else if (num >= 20 && num <= 30) {
					//move to RDY
					processor->ReqeueueRunningProcess();
				}
				else if (num >= 50 && num <= 60) {
					processor->TerminateRunningProcess();
				}
			}
		}

		Process* blkTopProc = 0;
		if (m_BlockedProcesses.Peek(&blkTopProc)) {
			if (RandomEngine::GetInt(1, 100) < 10) {
				//dequeue from BLK
				m_BlockedProcesses.Dequeue();

				//reschedule
				Schedule(blkTopProc);
			}
		}

		//update io
		UpdateIO();

		//mark updated
		m_SimulationInfo.NotifyUpdated();

		//check if simulation has ended
		if (m_TerminatedProcesses.GetLength() == m_LoadFileInfo.proc_count) {
			//done, stop the simulation
			m_SimulationInfo.Stop();
			m_View.NotifyStopped();
		}
	}

	void Scheduler::Schedule(Process* proc) {
		//get processor with shortest queue
		Processor* processor = GetProcessorWithShortestQueue();
		if (processor == 0) {
			//processor is null? this should never happen
			return;
		}

		//queue process
		processor->QueueProcess(proc);
	}

	void Scheduler::IncrementTimestep() {
		//we can only advance timestep in interactive mode
		if (m_SimulationInfo.GetMode() == SimulationMode::Interactive && m_SimulationInfo.GetState() == SimulationState::Playing) {
			m_SimulationInfo.IncrementTimestep();
		}
	}

	void Scheduler::LoadSerializedData(_STD wstring& filename) {
		//read input file
		Deserializer deserializer(filename);

		DeserializerData data;
		bool success;
		if (success = deserializer.Deserialize(data)) {
			//successfully deserialized data
			for (int i = 0; i < data.proc_count; i++) {
				m_NewProcesses.Enqueue(data.procs[i]);
			}

			//create processors
			for (int i = 0; i < data.num_processors_fcfs; i++) {
				m_Processors.Add(new ProcessorFCFS(this));
			}

			for (int i = 0; i < data.num_processors_sjf; i++) {
				m_Processors.Add(new ProcessorSJF(this));
			}

			for (int i = 0; i < data.num_processors_rr; i++) {
				m_Processors.Add(new ProcessorRR(this));
			}

			//enqueue sigkills
			for (int i = 0; i < deserializer.GetSigkillCount(); i++) {
				m_Sigkills.Enqueue(data.sigkills[i]);
			}
		}
		
		//update file load info
		m_LoadFileInfo = {
			filename,
			success,
			data.proc_count
		};
	}

	void Scheduler::NotifyProcessTerminated(Process* proc) {
		//add process to TRM list
		m_TerminatedProcesses.Add(proc);
	}

	void Scheduler::NotifyProcessBlocked(Process* proc) {
		//enqueue to BLK
		m_BlockedProcesses.Enqueue(proc);
	}

	void Scheduler::Print(_STD wstringstream& stream) {
		stream << L"RDY processes\n";

		int runCounter = 0; //count running procs
		for (int i = 0; i < m_Processors.GetLength(); i++) {
			Processor* processor = *m_Processors[i];
			processor->Print(stream);

			if (processor->GetState() == ProcessorState::BUSY) {
				runCounter++;
			}
		}

		//BLK
		stream << L"\nBLK processes\n";
		stream << m_BlockedProcesses.GetLength() << L" BLK: ";
		m_BlockedProcesses.Print(stream);

		//RUN
		stream << L"\nRUN processes\n";
		stream << runCounter << L" RUN: ";

		for (int i = 0; i < m_Processors.GetLength(); i++) {
			Processor* processor = *m_Processors[i];
			if (processor->GetRunningProcess() != 0) {
				stream << processor->GetRunningProcess()->GetPID() << L"(P" << (i + 1) << L"), ";
			}
		}

		//TRM
		stream << L"\nTRM processes\n";
		stream << m_TerminatedProcesses.GetLength() << L" TRM: ";
		m_TerminatedProcesses.Print(stream);

		//IO
		stream << L"\n\nIO mutex: ";
		if (m_IOMutex.owner != 0) {
			stream << L"owner(" << m_IOMutex.owner->GetPID() << L") dur(" << m_IOMutex.io_data.duration << L")";
		}
	}
}
