#pragma once

#include "states.h"

namespace core {
	class Processor {
	protected:
		/// <summary>
		/// Current processor state
		/// </summary>
		ProcessorState m_State;

	public:
		Processor();

		/// <summary>
		/// Returns the current processor state
		/// </summary>
		ProcessorState GetState();
	};
}