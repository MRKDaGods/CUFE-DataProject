#pragma once

#include "../common.h"
#include "../ui/gui.h"
#include "../collections/linked_list.h"
#include "../collections/array_list.h"
#include "../collections/linked_queue.h"
#include "processor.h"
#include "process.h"
#include "simulation_info.h"
#include "scheduler_view.h"
#include "deserializer.h"
#include "logger.h"

#include <string>

namespace core {
	/// <summary>
	/// Currently loaded file info
	/// </summary>
	struct LoadFileInfo {
		/// <summary>
		/// The file path (empty means not loaded yet)
		/// </summary>
		_STD wstring filename;

		/// <summary>
		/// Loaded successfully?
		/// </summary>
		bool success;

		//rest of struct is from DeserializerData

		//time slice for RR
		int rr_timeslice;

		//attribs
		int rtf;
		int maxw;
		int stl;
		int fork_prob;

		//number of processes
		int proc_count;
	};

	struct IOMutex {
		ProcessIOData io_data;
		Process* owner;
	};

	class Scheduler {
	private:
		/// <summary>
		/// The user interface controller
		/// </summary>
		_UI GUI m_UI;

		/// <summary>
		/// List of available processors
		/// </summary>
		_COLLECTION ArrayList<Processor*> m_Processors;

		/// <summary>
		/// Queue of NEW processes
		/// </summary>
		_COLLECTION LinkedQueue<Process*> m_NewProcesses;

		/// <summary>
		/// List of TRM process pids
		/// </summary>
		_COLLECTION LinkedList<int> m_TerminatedProcesses;

		/// <summary>
		/// Queue of BLK processes
		/// </summary>
		_COLLECTION ProcessLinkedQueue m_BlockedProcesses;

		/// <summary>
		/// Currently loaded file info
		/// </summary>
		LoadFileInfo m_LoadFileInfo;

		/// <summary>
		/// Simulation related info
		/// </summary>
		SimulationInfo m_SimulationInfo;

		/// <summary>
		/// View responsible for rendering the interface
		/// </summary>
		SchedulerView m_View;

		/// <summary>
		/// An IO mutex for processes to access IO
		/// </summary>
		IOMutex m_IOMutex;

		/// <summary>
		/// A general purpose logger, singleton lifetime is tied to Scheduler
		/// </summary>
		Logger m_Logger;

		/// <summary>
		/// Returns the processor with the shortest queue
		/// </summary>
		Processor* GetProcessorWithShortestQueue();

		/// <summary>
		/// Monitors the IO mutex
		/// </summary>
		void UpdateIO();

		/// <summary>
		/// Updates a processor
		/// </summary>
		void UpdateProcessor(Processor* processor);

	public:
		Scheduler();
		~Scheduler();

		/// <summary>
		/// Loaded file info
		/// </summary>
		LoadFileInfo* GetLoadFileInfo();

		/// <summary>
		/// Simulation related info
		/// </summary>
		SimulationInfo* GetSimulationInfo();

		/// <summary>
		/// Updates to the next frame
		/// </summary>
		void Update();

		/// <summary>
		/// Schedules a process to be ran on a processor
		/// </summary>
		void Schedule(Process* proc);

		/// <summary>
		/// Advances the timestep, if state is playing and in interactive mode
		/// </summary>
		void IncrementTimestep();

		/// <summary>
		/// Loads the processes and other info using the deserializer
		/// </summary>
		void LoadSerializedData(_STD wstring& filename);

		/// <summary>
		/// Notifies the scheduler that a process has been terminated 
		/// and should be moved the TRM list
		/// </summary>
		void NotifyProcessTerminated(Process* proc);

		/// <summary>
		/// Notifies the scheduler that a process has been blocked 
		/// and should be moved the BLK list
		/// </summary>
		void NotifyProcessBlocked(Process* proc);

		// Forks a new process
		void ForkProcess(Process* parent);

		/// <summary>
		/// Prints info from scheduler into the stream
		/// </summary>
		void Print(_STD wstringstream& stream);
	};
}