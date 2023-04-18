#pragma once

#include <fstream>
#include <string>

#include "process.h"

namespace core {
	/// <summary>
	/// Sigkill time related info
	/// </summary>
	struct SigkillTimeInfo {
		int time;
		int proc_pid;

		bool operator==(SigkillTimeInfo& other) {
			return time == other.time && proc_pid == other.proc_pid;
		}
	};

	/// <summary>
	/// Represents the data output from a deserializer
	/// </summary>
	struct DeserializerData {
		//number of processors
		int num_processors_fcfs;
		int num_processors_sjf;
		int num_processors_rr;
		
		//time slice for RR
		int rr_timeslice;

		//attribs
		int rtf;
		int maxw;
		int stl;
		int fork_prob;

		//number of processes
		int proc_count;

		//processes info
		Process** procs;

		//sig kill data
		SigkillTimeInfo* sigkills;
	};

	/// <summary>
	/// Deserializes an input file
	/// </summary>
	class Deserializer {
	private:
		/// <summary>
		/// Input file stream
		/// </summary>
		_STD ifstream m_Stream;

		/// <summary>
		/// Process array
		/// </summary>
		Process** m_Processes;
		
		/// <summary>
		/// Sigkill data
		/// </summary>
		SigkillTimeInfo* m_Sigkills;

		/// <summary>
		/// Number of sigkills
		/// </summary>
		int m_SigkillCount;

	public:
		Deserializer(_STD wstring& path);
		~Deserializer();

		/// <summary>
		/// Does the input file exist?
		/// </summary>
		bool IsValid();

		/// <summary>
		/// Number of sigkills
		/// </summary>
		int GetSigkillCount();

		bool Deserialize(DeserializerData& data);
	};
}