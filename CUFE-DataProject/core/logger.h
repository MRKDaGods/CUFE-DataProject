#pragma once

#include "../common.h"
#include "../ui/color.h"
#include "../collections/linked_list.h"

#include <string>
#include <mutex>

#define LOG(msg) core::Logger::GetInstance()->Log(msg)
#define LOGF(fmt, ...) { wchar_t __tmpBuf[100]; swprintf(__tmpBuf, fmt, __VA_ARGS__); LOG(__tmpBuf); }

namespace core {
	class Scheduler;

	struct LogMessage {
		_STD wstring text;
		_UI Color color;

		bool operator==(LogMessage& other) {
			return text == other.text && color == other.color;
		}
	};

	class Logger {
	private:
		/// <summary>
		/// List of log entries
		/// </summary>
		_COLLECTION LinkedList<LogMessage>* m_Logs;

		/// <summary>
		/// Max number of logs
		/// </summary>
		int m_MaxNumberOfLogs;

		/// <summary>
		/// The scheduler
		/// </summary>
		Scheduler* m_Scheduler;

		/// <summary>
		/// Current log color
		/// </summary>
		_UI Color m_Color;

		/// <summary>
		/// Log mutex
		/// </summary>
		_STD mutex m_LogMutex;

		/// <summary>
		/// Singleton instance
		/// </summary>
		static Logger* ms_Instance;

	public:
		Logger(int maxLogs, Scheduler* scheduler);
		~Logger();

		/// <summary>
		/// Returns the singleton instance
		/// </summary>
		static Logger* GetInstance();

		/// <summary>
		/// Returns the logs
		/// </summary>
		_COLLECTION LinkedList<LogMessage>* GetLogs();

		/// <summary>
		/// Adds a log entry
		/// </summary>
		void Log(LogMessage msg, bool acquireMutex = true);

		/// <summary>
		/// Adds a log entry
		/// </summary>
		void Log(_STD wstring msg);

		/// <summary>
		/// Sets a global color for the following logs
		/// </summary>
		void SetColor(_UI Color color);

		/// <summary>
		/// Prevents any thread from accessing the logs (locks the mutex)
		/// </summary>
		void AcquireMutex();

		/// <summary>
		/// Unlocks the mutex
		/// </summary>
		void ReleaseMutex();
	};
}