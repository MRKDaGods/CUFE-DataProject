#include "statistics.h"
#include "deserializer.h"
#include "scheduler.h"

#include <fstream>

namespace core {
	Statistics::Statistics(Scheduler* scheduler) : m_Scheduler(scheduler), m_FirstProcTime(-1), m_LastTime(0) {
		memset(m_Records, 0, sizeof(int) * (int)StatisticType::MAX);
	}

	int Statistics::GetAverageWaitingTime() {
		int totalWt = 0;

		for (int i = 0; i < m_Processes.GetLength(); i++) {
			totalWt += m_Processes[i]->waiting_time;
		}

		return totalWt / m_Processes.GetLength();
	}

	int Statistics::GetAverageResponseTime() {
		int totalRt = 0;

		for (int i = 0; i < m_Processes.GetLength(); i++) {
			totalRt += m_Processes[i]->response_time;
		}

		return totalRt / m_Processes.GetLength();
	}

	int Statistics::GetAverageTurnaroundDuration() {
		int totalTrt = 0;

		for (int i = 0; i < m_Processes.GetLength(); i++) {
			totalTrt += m_Processes[i]->turnaround_duration;
		}

		return totalTrt / m_Processes.GetLength();
	}

	int Statistics::GetAverageDeadline() {
		int totalDl = 0;

		for (int i = 0; i < m_Processes.GetLength(); i++) {
			totalDl += m_Processes[i]->deadline;
		}

		return totalDl / m_Processes.GetLength();
	}

	void Statistics::AddProcessStatistic(Process* proc) {
		m_Processes.Add(ProcessStatEntry{
			proc->GetTerminationTime(),
			proc->GetPID(),
			proc->GetArrivalTime(),
			proc->GetCPUTime(),
			proc->GetTotalIOTime(),
			proc->GetWaitingTime(),
			proc->GetResponseTime(),
			proc->GetTurnaroundDuration(),
			proc->GetDeadline()
		});
	}

	void Statistics::AddStatistic(StatisticType type) {
		if (type != StatisticType::MAX) {
			m_Records[(int)type]++;
		}
	}

	int Statistics::GetTotalTurnaroundDuration() {
		return m_LastTime - m_FirstProcTime;
	}

	void Statistics::SetFirstProcTime(int time) {
		m_FirstProcTime = time;
	}

	int Statistics::GetFirstProcTime() {
		return m_FirstProcTime;
	}

	void Statistics::SetLastTime(int time) {
		m_LastTime = time;
	}

	int Statistics::GetLastTime() {
		return m_LastTime;
	}

	void Statistics::WriteToFile(_COLLECTION ArrayList<Processor*>* processors, _STD string filename) {
		//write stats
		_STD ofstream stream(filename, _STD ios::out);

		//global buffer
		char buf[1024];
		
		//write header
		constexpr int headerCount = 9;

		const char* header[headerCount] = {
			"TT", "PID", "AT", "CT", "IO_D", "WT", "RT", "TRT", "DL"
		};

		for (int i = 0; i < headerCount; i++) {
			sprintf(buf, "%-10s", header[i]);
			stream << buf;
		}

		stream << '\n';

		int procCount = m_Processes.GetLength();

		//write procs
		for (int i = 0; i < procCount; i++) {
			ProcessStatEntry* stat = m_Processes[i];

			for (int j = 0; j < headerCount; j++) {
				sprintf(buf,
					"%-10d", 
					*(int*)((uintptr_t)stat + 0x4ULL * j));
				stream << buf;
			}

			stream << '\n';
		}

		stream << '\n';

		//proc count
		sprintf(buf, "Processes: %d\n", procCount);
		stream << buf;

		//stats
		sprintf(buf,
			"Avg WT = %d\t\t\tAvg RT = %d\t\tAvg TRT=%d\t\tAvg DL=%d\n", 
			GetAverageWaitingTime(),
			GetAverageResponseTime(),
			GetAverageTurnaroundDuration(),
			GetAverageDeadline());
		stream << buf;

		//migration
		sprintf(buf, 
			"Migration %%:\t\tRTF = %.0f%%\t\tMaxW = %.0f%%\n",
			m_Records[(int)StatisticType::MigrationRTF] / (float)procCount * 100.f,
			m_Records[(int)StatisticType::MigrationMaxW] / (float)procCount * 100.f);
		stream << buf;

		//steal
		sprintf(buf,
			"Work Steal %%: %.0f%%\n",
			m_Records[(int)StatisticType::WorkSteal] / (float)procCount * 100.f);
		stream << buf;

		//fork
		sprintf(buf,
			"Forked Process %%: %.0f%%\n",
			m_Records[(int)StatisticType::Fork] / (float)procCount * 100.f);
		stream << buf;

		//killed
		sprintf(buf,
			"Killed Process %%: %.0f%%\n",
			m_Records[(int)StatisticType::Kill] / (float)procCount * 100.f);
		stream << buf;

		stream << '\n';

		//processors
		_STD string processorsStr;
		{
			DeserializerData* data = &m_Scheduler->GetLoadFileInfo()->data;
			if (data->num_processors_fcfs > 0) {
				sprintf(buf, "%d FCFS, ", data->num_processors_fcfs);
				processorsStr += buf;
			}

			if (data->num_processors_sjf > 0) {
				sprintf(buf, "%d SJF, ", data->num_processors_sjf);
				processorsStr += buf;
			}

			if (data->num_processors_rr > 0) {
				sprintf(buf, "%d RR, ", data->num_processors_rr);
				processorsStr += buf;
			}

			if (data->num_processors_edf > 0) {
				sprintf(buf, "%d EDF, ", data->num_processors_edf);
				processorsStr += buf;
			}

			//check if we have any, if so remove trailing comma and space
			if (!processorsStr.empty()) {
				//remove trailing
				processorsStr = processorsStr.substr(0, processorsStr.size() - 2);
			}
		}

		sprintf(buf, "Processors: %d [%s]\n", processors->GetLength(), processorsStr.c_str());
		stream << buf;

		//load
		stream << "Processors Load\n";
		for (int i = 0; i < processors->GetLength(); i++) {
			sprintf(buf, 
				"P%d = %.2f%%%s\t",

				//processor id
				i + 1,

				//processor load
				(*(*processors)[i])->GetProcessorLoad() * 100.f,

				//trailing
				i < processors->GetLength() - 1 ? ", " : "");

			stream << buf;
		}

		stream << '\n';

		//utilization
		float totalUtil = 0;

		stream << "Processors Utilization\n";
		for (int i = 0; i < processors->GetLength(); i++) {
			float util = (*(*processors)[i])->GetProcessorUtilization();

			//increment util
			totalUtil += util;

			sprintf(buf,
				"P%d = %.2f%%%s\t",

				//processor id
				i + 1,

				//processor load
				util * 100.f,

				//trailing
				i < processors->GetLength() - 1 ? ", " : "");

			stream << buf;
		}

		sprintf(buf, "\nAvg utilization = %.1f%%", totalUtil / (float)processors->GetLength() * 100.f);
		stream << buf;

		stream.close();
	}
}