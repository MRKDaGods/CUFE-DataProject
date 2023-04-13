#pragma once

namespace core {
	enum class ProcessState {
		/// <summary>
		/// New
		/// </summary>
		NEW,

		/// <summary>
		/// Ready
		/// </summary>
		RDY,

		/// <summary>
		/// Running
		/// </summary>
		RUN,

		/// <summary>
		/// Blocked
		/// </summary>
		BLK,

		/// <summary>
		/// Terminated
		/// </summary>
		TRM,

		/// <summary>
		/// Orphan
		/// </summary>
		ORPH
	};

	enum class ProcessorState {
		/// <summary>
		/// Executing a process
		/// </summary>
		BUSY,

		/// <summary>
		/// No process to execute
		/// </summary>
		IDLE
	};
}