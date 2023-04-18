#include "simulation_info.h"

namespace core {
	SimulationInfo::SimulationInfo() : m_Mode(SimulationMode::Interactive), m_State(SimulationState::Stopped), m_Timestep(0), m_Dirty(false) {
		//we are initially in interactive mode and are stopped
	}

	SimulationMode SimulationInfo::GetMode() {
		return m_Mode;
	}

	SimulationState SimulationInfo::GetState() {
		return m_State;
	}

	int SimulationInfo::GetTimestep() {
		return m_Timestep;
	}

	bool SimulationInfo::CanUpdateScheduler() {
		if (m_State != SimulationState::Playing) return false;

		switch (m_Mode)
		{
		case core::SimulationMode::Interactive:
			return m_Dirty;

		case core::SimulationMode::StepByStep:
			break;

		case core::SimulationMode::Silent:
			break;
		}

		return false;
	}

	bool SimulationInfo::Start() {
		//check if we are playing already
		if (m_State == SimulationState::Playing) return false;

		m_State = SimulationState::Playing;
		m_Dirty = true;

		return true;
	}

	bool SimulationInfo::Stop() {
		if (m_State == SimulationState::Stopped) return false;

		//set state to stopped
		m_State = SimulationState::Stopped;

		//reset timestep
		m_Timestep = 0;

		return true;
	}

	bool SimulationInfo::Pause() {
		//we can only pause if we are playing
		if (m_State == SimulationState::Playing) {
			m_State = SimulationState::Paused;
			return true;
		}

		return false;
	}
	

	void SimulationInfo::IncrementTimestep() {
		m_Timestep++;
		m_Dirty = true;
	}

	void SimulationInfo::SetMode(SimulationMode& mode) {
		//stop then change the mode
		Stop();
		
		m_Mode = mode;
	}

	void SimulationInfo::NotifyUpdated() {
		m_Dirty = false;
	}

	_STD wstring SimulationModeToWString(SimulationMode mode) {
		switch (mode)
		{
		case SimulationMode::Interactive:
			return L"Interactive";

		case SimulationMode::StepByStep:
			return L"StepByStep";

		case SimulationMode::Silent:
			return L"Silent";
		}

		return L"";
	}
}
