#include "scheduler.h"
#include "deserializer.h"

namespace core {
	Scheduler::Scheduler() {
		//initialize ui controller
		m_UIController.Initialize(APP_NAME, _UTIL Vector2(APP_SIZE_WIDTH, APP_SIZE_HEIGHT));

		//read input file
		Deserializer deserializer("lolxd");

		DeserializerData data;
		if (!deserializer.Deserialize(data)) {
			//fail
			//later b2a
		}
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
}
