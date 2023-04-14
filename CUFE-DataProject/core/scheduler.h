#pragma once

#include "../common.h"
#include "../ui/gui.h"
#include "../collections/linked_list.h"
#include "../collections/linked_queue.h"
#include "processor.h"
#include "process.h"

namespace core {
	class Scheduler {
	private:
		/// <summary>
		/// The user interface controller
		/// </summary>
		_UI GUI m_UI;

		/// <summary>
		/// List of available processors
		/// </summary>
		_COLLECTION LinkedList<Processor*> m_Processors;

		/// <summary>
		/// Queue of NEW processes
		/// </summary>
		_COLLECTION LinkedQueue<Process*> m_NewProcesses;

		/// <summary>
		/// Has the deserialization completed successfully?
		/// </summary>
		bool m_LoadSuccess;

		/// <summary>
		/// Represents the current timestep
		/// </summary>
		int m_CurrentTimestep;

		/// <summary>
		/// Loads the processes and other info using the deserializer
		/// </summary>
		void LoadSerializedData();

		/// <summary>
		/// External UI frame callback
		/// </summary>
		void UICallback();

	public:
		Scheduler();
		~Scheduler();

		/// <summary>
		/// Updates to the next frame
		/// </summary>
		void Update();

		/// <summary>
		/// Schedules a process to be ran on a processor
		/// </summary>
		void Schedule(Process* proc);
	};
}