#include "logger.h"
#include "scheduler.h"

namespace core {
	Logger* Logger::ms_Instance = 0;

	Logger::Logger(int maxLogs, Scheduler* scheduler) : m_MaxNumberOfLogs(maxLogs), m_Scheduler(scheduler) {
		if (ms_Instance == 0) {
			ms_Instance = this;
		}

		//by default
		m_Color = COL(BLACK, WHITE);
		
		m_Logs = new _COLLECTION LinkedList<LogMessage>();
	}

	Logger::~Logger() {
		delete m_Logs;
	}

	_COLLECTION LinkedList<LogMessage>* Logger::GetLogs() {
		return m_Logs;
	}

	Logger* Logger::GetInstance() {
		return ms_Instance;
	}

	void Logger::Log(LogMessage msg, bool acquireMutex) {
		if (acquireMutex) { //prevent deadlock
			AcquireMutex();
		}

		constexpr int maxLen = LOG_WIDTH - 7;

		int ts = m_Scheduler->GetSimulationInfo()->GetTimestep();

		//remove first log
		if (m_Logs->GetLength() == m_MaxNumberOfLogs) {
			m_Logs->Remove(*(*m_Logs)[0]);
		}

		while (msg.text.length() > maxLen) {
			Log(LogMessage{ 
				msg.text.substr(0, maxLen),
				msg.color
			}, false);

			msg.text = msg.text.substr(maxLen);
		}

		if (msg.text.size() > 0) {
			wchar_t buf[LOG_WIDTH];
			swprintf(buf, L"[%d] %s", ts, msg.text.c_str());
			msg.text = buf;

			m_Logs->Add(msg);
		}

		if (acquireMutex) {
			ReleaseMutex();
		}
	}

	void Logger::Log(_STD wstring msg) {
		Log(LogMessage{ msg, m_Color });
	}

	void Logger::SetColor(_UI Color color) {
		m_Color = color;
	}

	void Logger::AcquireMutex() {
		//blocks thread till mutex is available
		m_LogMutex.lock();
	}

	void Logger::ReleaseMutex() {
		m_LogMutex.unlock();
	}
}