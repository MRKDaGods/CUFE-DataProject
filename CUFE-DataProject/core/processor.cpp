#include "processor.h"

namespace core {
	Processor::Processor() : m_State(ProcessorState::IDLE) {
	}

	ProcessorState Processor::GetState() {
		return m_State;
	}
}