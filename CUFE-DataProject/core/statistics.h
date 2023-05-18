#pragma once

#include "../common.h"
#include "../collections/linked_queue.h"
#include "../collections/array_list.h"
#include "process.h"
#include "processor.h"

#include <string>

namespace core {
	class Scheduler;
	
	struct ProcessStatEntry {
		int termination_time;
		int pid;
		int arrival_time;
		int cpu_time;
		int total_io_time;
		int waiting_time;
		int response_time;
		int turnaround_duration;
		int deadline;

		bool operator==(ProcessStatEntry& other) {
			return memcmp(this, &other, sizeof(ProcessStatEntry)) == 0;
		}
	};

	enum class StatisticType {
		//migration
		MigrationRTF,
		MigrationMaxW,

		//other
		WorkSteal,
		Fork,
		Kill,

		MAX
	};

	class Statistics {
	private:
		/// Pointer to scheduler
		Scheduler* m_Scheduler;

		/// Processes stat list
		/// we are not using Queue for the following reason
		///	we may want to calculate things like average waiting time, etc
		///		and we would need to keep track of the length of the stats, etc
		///		by the time we print the process stats, the queue will be empty
		///		thus we would have needed to keep track of each property seperately
		///		-- total waiting time, total rt, total trt, etc..
		///		it becomes messy and it defeats the purpose of storing the stats in a struct of its own
		_COLLECTION LinkedList<ProcessStatEntry> m_Processes;

		/// Statistic Records
		int m_Records[(int)StatisticType::MAX];

		int m_FirstProcTime;
		int m_LastTime;

		/// Returns the average waiting time of all processes
		int GetAverageWaitingTime();

		/// Returns the average response time of all processes
		int GetAverageResponseTime();

		/// Returns the average turnaround duration of all processes
		int GetAverageTurnaroundDuration();

		/// Returns the average deadline of all processes
		int GetAverageDeadline();

	public:
		Statistics(Scheduler* scheduler);

		/// Adds a process statistic
		void AddProcessStatistic(Process* proc);

		/// Incremets a statistic of certain type
		void AddStatistic(StatisticType type);

		/// Total TRT of process entries
		int GetTotalTurnaroundDuration();

		void SetFirstProcTime(int time);
		int GetFirstProcTime();

		void SetLastTime(int time);
		int GetLastTime();

		/// Writes the statistics to file
		void WriteToFile(_COLLECTION ArrayList<Processor*>* processors, _STD string filename);
	};
}