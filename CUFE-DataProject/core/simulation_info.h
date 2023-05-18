#pragma once

#include "../common.h"

#include <string>

namespace core {
	enum class SimulationMode {
		Interactive,
		StepByStep,
		Silent
	};

	_STD wstring SimulationModeToWString(SimulationMode mode);

	enum class SimulationState {
		Stopped,
		Playing,
		Paused
	};

	/// <summary>
	/// Contains simulation related info
	/// </summary>
	class SimulationInfo {
	private:
		/// <summary>
		/// The simulation mode
		/// </summary>
		SimulationMode m_Mode;

		/// <summary>
		/// The simulation state
		/// </summary>
		SimulationState m_State;

		/// <summary>
		/// Current timestep
		/// </summary>
		int m_Timestep;

		/// <summary>
		/// Dirty flag (interactive)
		/// </summary>
		bool m_Dirty;

	public:
		SimulationInfo();

		/// <summary>
		/// The current simulation mode
		/// </summary>
		/// <returns></returns>
		SimulationMode GetMode();

		/// <summary>
		/// The current simulation state
		/// </summary>
		/// <returns></returns>
		SimulationState GetState();

		/// <summary>
		/// Current timestep
		/// </summary>
		/// <returns></returns>
		int GetTimestep();

		/// <summary>
		/// Should the scheduler update?
		/// </summary>
		bool CanUpdateScheduler(int* sleepTime);

		/// <summary>
		/// Starts the simulation with respect to the current mode
		/// </summary>
		bool Start();

		/// <summary>
		/// Stops the simulation
		/// </summary>
		bool Stop();

		/// <summary>
		/// Pauses the simulation
		/// </summary>
		bool Pause();

		/// <summary>
		/// Increments the timestep
		/// </summary>
		void IncrementTimestep();

		/// <summary>
		/// Sets the current simulaiton mode
		/// </summary>
		void SetMode(SimulationMode& mode);

		/// <summary>
		/// Sets the dirty flag to false (the current frame has finished running)
		/// </summary>
		void NotifyUpdated();
	};
}