#include "logger.h"
#include "scheduler.h"

namespace core {
	Logger* Logger::ms_Instance = 0;

	Logger::Logger(int maxLogs, Scheduler* scheduler) : m_MaxNumberOfLogs(maxLogs), m_Scheduler(scheduler) {
		if (ms_Instance == 0) {
			ms_Instance = this;
		}
		
		m_Logs = new _COLLECTION LinkedList<_STD wstring>();
	}

	Logger::~Logger() {
		delete m_Logs;
	}

	_COLLECTION LinkedList<_STD wstring>* Logger::GetLogs() {
		return m_Logs;
	}

	Logger* Logger::GetInstance() {
		return ms_Instance;
	}

	void Logger::Log(_STD wstring msg) {
		constexpr int maxLen = LOG_WIDTH - 7;

		int ts = m_Scheduler->GetSimulationInfo()->GetTimestep();

		//remove first log
		if (m_Logs->GetLength() == m_MaxNumberOfLogs) {
			m_Logs->Remove(*(*m_Logs)[0]);
		}

		while (msg.length() > maxLen) {
			Log(msg.substr(0, maxLen));
			msg = msg.substr(maxLen);
		}

		if (msg.size() > 0) {
			wchar_t buf[LOG_WIDTH];
			swprintf(buf, L"[%d] %s", ts, msg.c_str());

			m_Logs->Add(buf);
		}
	}
}