#pragma once

#include "../collections/linked_list.h"
#include "../common.h"

#include <string>

#define LOG(msg) core::Logger::GetInstance()->Log(msg)

namespace core {
	class Scheduler;

	class Logger {
	private:
		/// <summary>
		/// List of log entries
		/// </summary>
		_COLLECTION LinkedList<_STD wstring>* m_Logs;

		/// <summary>
		/// Max number of logs
		/// </summary>
		int m_MaxNumberOfLogs;

		/// <summary>
		/// The scheduler
		/// </summary>
		Scheduler* m_Scheduler;

		/// <summary>
		/// Singleton instance
		/// </summary>
		static Logger* ms_Instance;

	public:
		Logger(int maxLogs, Scheduler* scheduler);
		~Logger();

		/// <summary>
		/// Returns the logs
		/// </summary>
		_COLLECTION LinkedList<_STD wstring>* GetLogs();

		/// <summary>
		/// Adds a log entry
		/// </summary>
		void Log(_STD wstring msg);

		/// <summary>
		/// Returns the singleton instance
		/// </summary>
		static Logger* GetInstance();
	};
}