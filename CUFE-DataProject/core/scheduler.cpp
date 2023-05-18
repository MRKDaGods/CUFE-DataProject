#include "scheduler.h"
#include "deserializer.h"
#include "processor_fcfs.h"
#include "processor_sjf.h"
#include "processor_rr.h"
#include "processor_edf.h"
#include "random_engine.h"

namespace core {
	Scheduler::Scheduler() : m_View(this, &m_UI), m_Logger(50, this), m_Statistics(this) {
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

	Processor* Scheduler::GetProcessorWithShortestQueue(ProcessorType processorType, Processor* exclude) {
		Processor* proc = 0;
		int val = 0;

		for (int i = 0; i < m_Processors.GetLength(); i++) {
			Processor* cur = *m_Processors[i];

			if (processorType != ProcessorType::None && processorType != cur->GetProcessorType()) {
				continue;
			}

			//if we are suspended, we cant queue obviously
			if (cur->GetState() == ProcessorState::STOP || cur == exclude) {
				continue;
			}

			if (proc == 0 || cur->GetConcurrentTimer(false) < val) {
				proc = cur;
				val = cur->GetConcurrentTimer(false);
			}
		}

		return proc;
	}

	void Scheduler::UpdateIO() {
		LOG(L"Updating IO...");

		//handle current execution
		if (m_IOMutex.owner != 0) {
			//decrement duration
			m_IOMutex.io_data.duration--;

			LOGF(L"Mutex owner exists, pid=%d, dur=%d", m_IOMutex.owner->GetPID(), m_IOMutex.io_data.duration);

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

				LOGF(L"Acquiring mutex, pid=%d, dur=%d", m_IOMutex.owner->GetPID(), m_IOMutex.io_data.duration);
			}
		}
	}

	void Scheduler::UpdateProcessor(Processor* processor) {
		//currently running process is not null, check for completion

		//only update processor if it's not in STOP
		if (processor->GetState() == ProcessorState::STOP) {
			//check stop timer
			processor->UpdateStateTimer();

			//reset timer if we're done
			if (processor->GetStateTime(ProcessorState::STOP) >= m_LoadFileInfo.data.overheat_delay) {
				processor->ResetStateTimer(ProcessorState::STOP);

				//go back to idle
				processor->SetState(ProcessorState::IDLE);
			}
			else 
				return; //only exit if we are still in STOP
		}

		Process* runningProc = processor->GetRunningProcess();
		if (runningProc != 0) {
			runningProc->Tick(m_SimulationInfo.GetTimestep());

			LOGF(L"Running proc id=%d, ticks=%d", runningProc->GetPID(), runningProc->GetTicks());

			//decrement timer by 1 tick
			processor->DecrementTimer();

			LOGF(L"Processor time left=%d", processor->GetConcurrentTimer());

			LOGF(L"IsDone=%s, HasIOEvent=%s", BOOL_TO_WSTR(runningProc->IsDone()), BOOL_TO_WSTR(runningProc->HasIOEvent()));

			//check if proc has finished executing
			if (runningProc->IsDone()) {
				LOG(L"Terminate current proc requested (isdone=true)");
				processor->TerminateRunningProcess();
			}
			else if (runningProc->HasIOEvent()) { //check for IO
				LOG(L"Terminate current proc requested (hasioevent=true)");

				//block process
				processor->BlockRunningProcess();
			}
		}

		//call schedule algo
		processor->ScheduleAlgo();

		//update state timer
		processor->UpdateStateTimer();

		//should we overheat?
		processor->CheckOverheat();
	}

	void Scheduler::Terminate() {
		LOG(L"Terminating scheduler...");

		m_Statistics.SetLastTime(m_SimulationInfo.GetTimestep());

		//done, stop the simulation
		m_SimulationInfo.Stop();
		m_View.NotifyStopped();

		LOG(L"Writing stats...");
		m_Statistics.WriteToFile(&m_Processors, "otest.txt");
		LOG(L"DONE");
	}

	void Scheduler::UpdateWorkStealing() {
		//check for STL time
		if (m_SimulationInfo.GetTimestep() % m_LoadFileInfo.data.stl != 0) return;

		if (m_Processors.GetLength() <= 1) {
			//no work stealing for such time
			return;
		}

		PUSHCOL(COL(DARK_BLUE, WHITE));
		LOG(L"Updating Work Stealing");

		//get longest queue and shortest queue
		struct {
			Processor* processor;
			int time;

			int id; //for debug
		} min = { 0, INT_MAX }, max = { 0, INT_MIN };

		for (int i = 0; i < m_Processors.GetLength(); i++) {
			Processor* cur = *m_Processors[i];
			
			//get remaining time without running proc
			int time = cur->GetConcurrentTimer(false);

			if (min.processor == 0 || min.time > time) {
				min.processor = cur;
				min.time = time;
				min.id = i + 1;
			}

			if (max.processor == 0 || max.time < time) {
				max.processor = cur;
				max.time = time;
				max.id = i + 1;
			}
		}

		//check if min = max
		if (min.processor != max.processor) {
			LOGF(L"Entering stealing, MIN=%d, MAX=%d", min.id, max.id);

			//debug
			int stealCount = 0;

			//start work stealing
			while (true) {
				//calc steal limit
				float stealLimit = (max.time - min.time) / (float)max.time;
				LOGF(L"StealLimit=%.2f%%", stealLimit * 100.f);
				if (stealLimit <= 0.4f) {
					//stealLimit isnt greater than 40%, no stealing
					LOG(L"Exiting stealing, low limit");
					break;
				}

				//stealLimit > 40%, start stealing
				//find applicable process to steal from longest
				//get steal handle
				StealHandle handle;
				if (!max.processor->GetStealHandle(&handle)) {
					LOG(L"Cannot obtain a steal handle, exiting...");
					break;
				}

				//we have a steal handle
				//execute handle, then queue process to min queue
				handle.execute();

				//queue process
				min.processor->QueueProcess(handle.process);

				//update min, max times
				min.time = min.processor->GetConcurrentTimer(false);
				max.time = max.processor->GetConcurrentTimer(false);

				//increment debug counter
				stealCount++;

				LOGF(L"Executed handle, stole pid=%d", handle.process->GetPID());

				//increment statistic
				ProcessDynamicMetadata* metadata = handle.process->GetDynamicMetadata();
				if (!metadata->stolen) {
					//mark stolen
					metadata->stolen = true;

					m_Statistics.AddStatistic(StatisticType::WorkSteal);
				}
			}

			LOGF(L"Steal finished, count=%d", stealCount);
		}

		LOG(L"Working stealing done");
		POPCOL();
	}

	int Scheduler::GetNumberOfActiveProcessors(ProcessorType type) {
		int count = 0;

		for (int i = 0; i < m_Processors.GetLength(); i++) {
			Processor* cur = *m_Processors[i];
			if ((type == ProcessorType::None || cur->GetProcessorType() == type) && cur->GetState() != ProcessorState::STOP) {
				count++;
			}
		}

		return count;
	}

	_STD wstring Scheduler::GetStatusbarText() {
		wchar_t buf[100];
		swprintf(buf, L"Active Processors(%d) TRM(%d/%d)", GetNumberOfActiveProcessors(ProcessorType::None), m_TerminatedProcesses.GetLength(), m_LoadFileInfo.data.proc_count);
		return _STD wstring(buf);
	}

	LoadFileInfo* Scheduler::GetLoadFileInfo() {
		return &m_LoadFileInfo;
	}

	SimulationInfo* Scheduler::GetSimulationInfo() {
		return &m_SimulationInfo;
	}

	Statistics* Scheduler::GetStatistics() {
		return &m_Statistics;
	}

	_UTIL Lock* Scheduler::GetSchedulerLock() {
		return &m_SchedulerLock;
	}

	void Scheduler::Update() {
		//check for processor count, obv dont run if there are no processors
		if (m_Processors.GetLength() == 0) {
			PUSHCOL(COL(DARK_RED, WHITE));
			LOG(L"No processors found !!!");
			POPCOL();

			Terminate();
			return;
		}

		//lock scheduler
		m_SchedulerLock.Acquire();

		int ts = m_SimulationInfo.GetTimestep();

		//set log color
		PUSHCOL(ts % 2 == 0 ? COL(GREY, BLACK) : COL(DARK_GREY, WHITE));

		LOG(L"Scheduler update started");

		LOGF(L"New proc count=%d", m_NewProcesses.GetLength());

		//get proc at current timestep
		Process* proc = 0;
		while (m_NewProcesses.Peek(&proc) && proc->GetArrivalTime() == ts) {
			//dequeue the proc
			m_NewProcesses.Dequeue();

			LOGF(L"Dequeued proc from NEW, pid=%d", proc->GetPID());

			//schedule it
			Schedule(proc);

			if (m_Statistics.GetFirstProcTime() == -1) {
				m_Statistics.SetFirstProcTime(ts);
			}
		}

		LOGF(L"Updating processors, count=%d", m_Processors.GetLength());

		//update processors
		for (int i = 0; i < m_Processors.GetLength(); i++) {
			Processor* processor = *m_Processors[i];

			LOGF(L"Updating processor ID=%d, type=%s", i + 1, ProcessorTypeToWString(processor->GetProcessorType()).c_str());

			UpdateProcessor(processor);
		}

		//update io
		UpdateIO();

		//work stealing
		UpdateWorkStealing();

		LOG(L"Scheduler update finished, notifying observers..");

		//mark updated
		m_SimulationInfo.NotifyUpdated();

		//check if simulation has ended
		if (m_TerminatedProcesses.GetLength() == m_LoadFileInfo.data.proc_count) {
			Terminate();
		}

		//pop logger color
		POPCOL();

		//unlock scheduler
		m_SchedulerLock.Release();
	}

	void Scheduler::Schedule(Process* proc, ProcessorType processorType, Processor* exclude) {
		LOG(L"Scheduling process, pid=" + _STD to_wstring(proc->GetPID()));

		//get processor with shortest queue
		Processor* processor = GetProcessorWithShortestQueue(processorType, exclude);
		if (processor == 0) {
			//processor is null? this should never happen
			//AMMAR: this is going to happen with overheat
			//edit: added CanOverheat
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
		LOGF(L"Loading serialized data, filename=%s", filename.c_str());

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

			LOGF(L"Created %d processes", data.proc_count);

			//reserve memory
			m_Processors.Reserve(data.num_processors_fcfs + data.num_processors_sjf + data.num_processors_rr);

			//create processors
			for (int i = 0; i < data.num_processors_fcfs; i++) {
				m_Processors.Add(new ProcessorFCFS(this));
			}

			LOGF(L"Created %d FCFS", data.num_processors_fcfs);

			for (int i = 0; i < data.num_processors_sjf; i++) {
				m_Processors.Add(new ProcessorSJF(this));
			}

			LOGF(L"Created %d SJF", data.num_processors_sjf);

			for (int i = 0; i < data.num_processors_rr; i++) {
				m_Processors.Add(new ProcessorRR(this));
			}

			LOGF(L"Created %d RR", data.num_processors_rr);

			for (int i = 0; i < data.num_processors_edf; i++) {
				m_Processors.Add(new ProcessorEDF(this));
			}

			LOGF(L"Created %d EDF", data.num_processors_edf);

			//enqueue sigkills
			for (int i = 0; i < deserializer.GetSigkillCount(); i++) {
				ProcessorFCFS::RegisterSigkillInfo(data.sigkills[i]);
			}

			LOGF(L"Created %d SIGKILLS", deserializer.GetSigkillCount());
		}
		else {
			LOG(L"Loading file failed");
		}
		
		//update file load info
		m_LoadFileInfo = {
			filename,
			success,

			//copy of deserialized data, DeserializerData::procs and DeserializerData::sigkills are invalid in this context
			data
		};
	}

	void Scheduler::NotifyProcessTerminated(Process* proc) {
		LOGF(L"Terminated process notif, pid=%d", proc->GetPID());

		//add process to TRM list
		m_TerminatedProcesses.Add(proc->GetPID());

		//calc stats
		proc->SetTerminationTime(m_SimulationInfo.GetTimestep());

		//add stat
		m_Statistics.AddProcessStatistic(proc);

		ForkingData* forkingData = proc->GetForkingData();

		if (forkingData->forgein_node) {
			//remove myself from foreign tree
			*forkingData->forgein_node = 0;
		}

		//kill my own children
		//i love cookies
		forkingData->Iterate([&](PROC_BT_NODE* node) {
			Process* child = node->value;

			//owner is not null here, must be FCFS
			//sanity check
			ProcessorFCFS* fcfs = dynamic_cast<ProcessorFCFS*>(child->GetOwner());
			if (fcfs == 0) {
				PUSHCOL(COL(RED, WHITE));
				LOGF(L"Fatal error, forked proc owner isnt fcfs, pid=%d", child->GetPID());
				POPCOL();

				return;
			}

			//set orph
			child->SetState(ProcessState::ORPH);

			//kill child
			fcfs->KillProcess(child->GetPID());
		});

		//delete process
		delete proc;
	}

	void Scheduler::NotifyProcessBlocked(Process* proc) {
		LOGF(L"Blocked process notif, pid=%d", proc->GetPID());

		//enqueue to BLK
		m_BlockedProcesses.Enqueue(proc);
	}

	void Scheduler::ForkProcess(Process* parent) {
		if (parent == 0 || !parent->CanFork()) return; //parent must not be null for a forked process

		LOGF(L"Forking new process, parent pid=%d", parent->GetPID());

		//create new process
		Process* child = new Process(++m_LoadFileInfo.data.proc_count,
			m_SimulationInfo.GetTimestep(),
			parent->GetRemainingTime(),
			0,
			0);

		LOGF(L"Child proc pid=%d", child->GetPID());

		//assign child and parent info
		ForkingData* parentForkingData = parent->GetForkingData();
		ForkingData* childForkingData = child->GetForkingData();

		parentForkingData->fork_tree.InsertForeign(&parent,
			childForkingData->fork_tree.GetRoot(),
			&childForkingData->forgein_node);

		//debug
		{
			int numForked = 0;
			parentForkingData->Iterate([&numForked](PROC_BT_NODE* node) {
				numForked++;
			});

			LOGF(L"Number of forked children=%d", numForked);
		}

		//schedule child process
		Schedule(child, ProcessorType::FCFS);

		//increment statistic
		m_Statistics.AddStatistic(StatisticType::Fork);
	}

	void Scheduler::MigrateProcess(Process* proc, ProcessorType targetProcessorType) {
		//caller has already ensured that targetProcessorType exists
		if (proc == 0) return;

		PUSHCOL(COL(DARK_RED, WHITE));
		LOGF(L"Migrating proc %d to %s", proc->GetPID(), ProcessorTypeToWString(targetProcessorType).c_str());
		POPCOL();

		//schedule to some other processor
		Schedule(proc, targetProcessorType);

		//increment statistic

		ProcessDynamicMetadata* metadata = proc->GetDynamicMetadata();
		if (!metadata->migrated) {
			metadata->migrated = true;

			switch (targetProcessorType) {
			case ProcessorType::SJF:
				//RTF
				m_Statistics.AddStatistic(StatisticType::MigrationRTF);
				break;

			case ProcessorType::RR:
				//MaxW
				m_Statistics.AddStatistic(StatisticType::MigrationMaxW);
				break;

			default:
				//unknown migration?
				PUSHCOL(COL(DARK_RED, WHITE));
				LOGF(L"UNKNOWN MIGRATION, type=%s", ProcessorTypeToWString(targetProcessorType).c_str());
				POPCOL();

				//mark unmigrated
				metadata->migrated = false;

				break;
			}
		}
	}

	bool Scheduler::CanProcessorOverheat(ProcessorType type) {
		//okay so we have many cases

		int numFcfs = GetNumberOfActiveProcessors(ProcessorType::FCFS);
		int numRR = GetNumberOfActiveProcessors(ProcessorType::RR);
		int numSJF = GetNumberOfActiveProcessors(ProcessorType::SJF);
		
		//RR cant overheat if there is a FCFS processor that might migrate to it, if it's the only active RR processor
		if (type == ProcessorType::RR && numRR == 1) {
			if (numFcfs > 0) {
				return false;
			}
		}

		//SJF cant overheat if there is an RR processor that might migrate to it, if it's the only active SJF processor
		if (type == ProcessorType::SJF && numSJF == 1) {
			if (numRR > 0) {
				return false;
			}
		}

		//generally a processor cant overheat if there are no other processors to run the processes
		//assume the calling processor still hasnt overheated, which makes them active wrt to their type
		//so decrement number of other active processors by 1
		return GetNumberOfActiveProcessors(ProcessorType::None) > 1;
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
