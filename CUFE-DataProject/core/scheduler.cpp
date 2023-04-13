#include "scheduler.h"

namespace core {
	Scheduler::~Scheduler() {
		//de alloc processors
		while (!m_Processors.IsEmpty()) {
			Processor* processor = *m_Processors[0];

			delete processor;

			m_Processors.Remove(processor);
		}
	}
}
