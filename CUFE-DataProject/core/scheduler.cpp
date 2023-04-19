#include "scheduler.h"
#include "deserializer.h"
#include "processor_fcfs.h"
#include "processor_sjf.h"
#include "processor_rr.h"
#include "random_engine.h"

namespace core {
	Scheduler::Scheduler() : m_View(this, &m_UI), m_Logger(50, this) {
		//initialize ui controller
		m_UI.Initialize(APP_NAME, _UTIL Vector2(APP_SIZE_WIDTH, APP_SIZE_HEIGHT), _STD bind(&SchedulerView::UICallback, &m_View));

		//zero out load file info
		memset(&m_LoadFileInfo, 0, sizeof(LoadFileInfo));

		//zero out io mutex
		memset(&m_IOMutex, 0, sizeof(IOMutex));
	}
	
	Scheduler::~Scheduler() {
		//delete processors
		for (int i = 0; i < m_Processors.GetLength(); i++) {
			delete *m_Processors[i];
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
		LOG(L"Updating IO...");

		wchar_t buf[100];

		//handle current execution
		if (m_IOMutex.owner != 0) {
			//decrement duration
			m_IOMutex.io_data.duration--;

			swprintf(buf, L"Mutex owner exists, pid=%d, dur=%d", m_IOMutex.owner->GetPID(), m_IOMutex.io_data.duration);
			LOG(buf);

			//we have finished
			if (m_IOMutex.io_data.duration == 0) {
				LOG(L"Time up, rescheduling mutex owner");

				//process should be scheduled again
				Schedule(m_IOMutex.owner);

				LOG(L"Releasing mutex");

				//clean mutex
				memset(&m_IOMutex, 0, sizeof(IOMutex));
			}
		}

		//check for another IO operation
		if (m_IOMutex.owner == 0) {
			LOG(L"Finding BLK proc for mutex");

			//find potential owner in BLK list
			if (m_BlockedProcesses.Dequeue(&m_IOMutex.owner)) {
				m_IOMutex.io_data = m_IOMutex.owner->GetIOData();

				swprintf(buf, L"Acquiring mutex, pid=%d, dur=%d", m_IOMutex.owner->GetPID(), m_IOMutex.io_data.duration);
				LOG(buf);
			}
		}
	}

	void Scheduler::UpdateProcessor(Processor* processor) {
		wchar_t buf[100];

		//currently running process is not null, check for completion
		Process* runningProc = processor->GetRunningProcess();
		if (runningProc != 0) {
			runningProc->Tick();

			swprintf(buf, L"Running proc id=%d, ticks=%d", runningProc->GetPID(), runningProc->GetTicks());
			LOG(buf);

			//decrement timer by 1 tick
			processor->DecrementTimer();

			swprintf(buf, L"Processor time left=%d", processor->GetConcurrentTimer());
			LOG(buf);

			swprintf(buf, L"IsDone=%s, HasIOEvent=%s", BOOL_TO_WSTR(runningProc->IsDone()), BOOL_TO_WSTR(runningProc->HasIOEvent(m_SimulationInfo.GetTimestep())));
			LOG(buf);

			//check if proc has finished executing
			if (runningProc->IsDone()) {
				LOG(L"Terminate current proc requested (isdone=true)");
				processor->TerminateRunningProcess();
			}
			else if (runningProc->HasIOEvent(m_SimulationInfo.GetTimestep())) { //check for IO
				LOG(L"Terminate current proc requested (hasioevent=true)");

				//block process
				processor->BlockRunningProcess();
			}
		}

		//get running process again incase of change
		//run schedule algo incase of process null
		if (processor->GetRunningProcess() == 0) {
			LOG(L"Processor doesnt have proc, scheduling...");
			processor->ScheduleAlgo();
		}

		//kill random proc in RDY for FCFS
		if (processor->GetProcessorType() == ProcessorType::FCFS) {
			LOG(L"Processor is FCFS");

			ProcessorFCFS* fcfs = (ProcessorFCFS*)processor;
			fcfs->KillRandomProcess();

			LOG(L"Looking for sigkill");
			//process sigkill for FCFS
			SigkillTimeInfo sigkill;
			if (m_Sigkills.Peek(&sigkill) && sigkill.time == m_SimulationInfo.GetTimestep()) {
				//dequeue sigkill
				m_Sigkills.Dequeue();

				swprintf(buf, L"Found sigkill for proc pid=%d", sigkill.proc_pid);
				LOG(buf);

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
		LOG(L"Scheduler update started");

		wchar_t buf[100];
		swprintf(buf, L"New proc count=%d", m_NewProcesses.GetLength());
		LOG(buf);

		//get proc at current timestep
		int ts = m_SimulationInfo.GetTimestep();
		Process* proc = 0;
		while (m_NewProcesses.Peek(&proc) && proc->GetArrivalTime() == ts) {
			//dequeue the proc
			m_NewProcesses.Dequeue();

			swprintf(buf, L"Dequeued proc from NEW, pid=%d", proc->GetPID());
			LOG(buf);

			//schedule it
			Schedule(proc);
		}

		swprintf(buf, L"Updating processors, count=%d", m_Processors.GetLength());
		LOG(buf);

		//update processors
		for (int i = 0; i < m_Processors.GetLength(); i++) {
			Processor* processor = *m_Processors[i];

			swprintf(buf, L"Updating processor ID=%d, type=%s", i + 1, ProcessorTypeToWString(processor->GetProcessorType()).c_str());
			LOG(buf);

			UpdateProcessor(processor);

			Process* proc = processor->GetRunningProcess();
			if (proc != 0) {
				LOG(L"Running proc exists, running probability");

				int num = RandomEngine::GetInt(1, 100);
				swprintf(buf, L"Num(1-100)=%d", num);
				LOG(buf);
				
				if (num >= 1 && num <= 15) {
					LOG(L"Res=MOVE TO BLK");

					//move to BLK
					//proc must have io data
					if (proc->HasAnyIOEvent()) {
						LOG(L"Blocing proc...");
						processor->BlockRunningProcess();
					}
				}
				else if (num >= 20 && num <= 30) {
					LOG(L"Res=MOVE TO RDY");

					//move to RDY
					processor->ReqeueueRunningProcess();
				}
				else if (num >= 50 && num <= 60) {
					LOG(L"Res=MOVE TO TRM");

					processor->TerminateRunningProcess();
				}
			}
		}

		LOG(L"BLK->RDY prob");
		Process* blkTopProc = 0;
		if (m_BlockedProcesses.Peek(&blkTopProc)) {
			LOG(L"BLK top proc found");
			if (RandomEngine::GetInt(1, 100) < 10) {
				LOG(L"Moving...");

				//dequeue from BLK
				m_BlockedProcesses.Dequeue();

				//reschedule
				Schedule(blkTopProc);
			}
		}

		//update io
		UpdateIO();

		LOG(L"Scheduler update finished, notifying observers..");

		//mark updated
		m_SimulationInfo.NotifyUpdated();

		//check if simulation has ended
		if (m_TerminatedProcesses.GetLength() == m_LoadFileInfo.proc_count) {
			LOG(L"Terminating scheduler...");

			//done, stop the simulation
			m_SimulationInfo.Stop();
			m_View.NotifyStopped();
		}
	}

	void Scheduler::Schedule(Process* proc) {
		LOG(L"Scheduling process, pid=" + _STD to_wstring(proc->GetPID()));

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
			LOG(L"Incrementing timestep...");

			m_SimulationInfo.IncrementTimestep();
		}
	}

	void Scheduler::LoadSerializedData(_STD wstring& filename) {
		wchar_t buf[100];
		swprintf(buf, L"Loading serialized data, filename=%s", filename.c_str());
		LOG(buf);

		//read input file
		Deserializer deserializer(filename);

		DeserializerData data;
		bool success;
		if (success = deserializer.Deserialize(data)) {
			LOG(L"Loading success, initializing data...");

			//successfully deserialized data
			for (int i = 0; i < data.proc_count; i++) {
				m_NewProcesses.Enqueue(data.procs[i]);
			}

			swprintf(buf, L"Created %d processes", data.proc_count);
			LOG(buf);

			//reserve memory
			m_Processors.Reserve(data.num_processors_fcfs + data.num_processors_sjf + data.num_processors_rr);

			//create processors
			for (int i = 0; i < data.num_processors_fcfs; i++) {
				m_Processors.Add(new ProcessorFCFS(this));
			}

			swprintf(buf, L"Created %d FCFS", data.num_processors_fcfs);
			LOG(buf);

			for (int i = 0; i < data.num_processors_sjf; i++) {
				m_Processors.Add(new ProcessorSJF(this));
			}

			swprintf(buf, L"Created %d SJF", data.num_processors_sjf);
			LOG(buf);

			for (int i = 0; i < data.num_processors_rr; i++) {
				m_Processors.Add(new ProcessorRR(this));
			}

			swprintf(buf, L"Created %d RR", data.num_processors_rr);
			LOG(buf);

			//enqueue sigkills
			for (int i = 0; i < deserializer.GetSigkillCount(); i++) {
				m_Sigkills.Enqueue(data.sigkills[i]);
			}

			swprintf(buf, L"Created %d SIGKILLS", deserializer.GetSigkillCount());
			LOG(buf);
		}
		else {
			LOG(L"Loading file failed");
		}
		
		//update file load info
		m_LoadFileInfo = {
			filename,
			success,
			data.proc_count
		};
	}

	void Scheduler::NotifyProcessTerminated(Process* proc) {
		wchar_t buf[100];
		swprintf(buf, L"Terminated process notif, pid=%d", proc->GetPID());
		LOG(buf);

		//add process to TRM list
		m_TerminatedProcesses.Add(proc->GetPID());

		//delete process
		delete proc;
	}

	void Scheduler::NotifyProcessBlocked(Process* proc) {
		wchar_t buf[100];
		swprintf(buf, L"Blocked process notif, pid=%d", proc->GetPID());
		LOG(buf);

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
		stream << L"\n\n\nBLK processes\n";
		stream << m_BlockedProcesses.GetLength() << L" BLK: ";
		m_BlockedProcesses.Print(stream);

		//RUN
		stream << L"\n\n\nRUN processes\n";
		stream << runCounter << L" RUN: ";

		for (int i = 0; i < m_Processors.GetLength(); i++) {
			Processor* processor = *m_Processors[i];
			if (processor->GetRunningProcess() != 0) {
				stream << processor->GetRunningProcess()->GetPID() << L"(P" << (i + 1) << L"), ";
			}
		}

		//TRM
		stream << L"\n\n\nTRM processes\n";
		stream << m_TerminatedProcesses.GetLength() << L" TRM: ";

		for (_COLLECTION LinkedListNode<int>* node = m_TerminatedProcesses.GetHead(); node; node = node->next) {
			stream << node->value << L", ";
		}

		//IO
		stream << L"\n\n\n\nIO mutex: ";
		if (m_IOMutex.owner != 0) {
			stream << L"owner(" << m_IOMutex.owner->GetPID() << L") dur(" << m_IOMutex.io_data.duration << L")";
		}
	}
}
