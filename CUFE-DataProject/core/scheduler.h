#pragma once

#include "../common.h"
#include "../collections/linked_list.h"
#include "../collections/linked_queue.h"
#include "processor.h"
#include "process.h"

namespace core {
	class Scheduler {
	private:
		/// <summary>
		/// List of available processors
		/// </summary>
		_COLLECTION LinkedList<Processor*> m_Processors;

		/// <summary>
		/// Queue of NEW processes
		/// </summary>
		_COLLECTION LinkedQueue<Process*> m_NewProcesses;

	public:
		~Scheduler();


	};
}