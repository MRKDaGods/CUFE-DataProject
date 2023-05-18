#pragma once

#include "../common.h"
#include "../ui/color.h"
#include "../collections/linked_list.h"
#include "../collections/linked_stack.h"
#include "../utils/lock.h"

#include <string>

#define LOG(msg) core::Logger::GetInstance()->Log(msg)
#define LOGF(fmt, ...) { wchar_t __tmpBuf[100]; swprintf(__tmpBuf, fmt, __VA_ARGS__); LOG(__tmpBuf); }

#define PUSHCOL(col) core::Logger::GetInstance()->PushColor(col)
#define POPCOL() core::Logger::GetInstance()->PopColor()

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

		// Color stack
		_COLLECTION LinkedStack<_UI Color> m_ColorStack;

		/// Log lock
		_UTIL Lock m_LoggerLock;

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

		// Pushes a new color to the stack
		void PushColor(_UI Color color);

		// Pops a color from the stack
		void PopColor();

		/// Returns the logger lock
		_UTIL Lock* GetLoggerLock();
	};
}