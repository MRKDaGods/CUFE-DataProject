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
		_UI GUI m_UIController;

		/// <summary>
		/// List of available processors
		/// </summary>
		_COLLECTION LinkedList<Processor*> m_Processors;

		/// <summary>
		/// Queue of NEW processes
		/// </summary>
		_COLLECTION LinkedQueue<Process*> m_NewProcesses;

	public:
		Scheduler();
		~Scheduler();
	};
}