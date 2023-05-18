#include "input_generator.h"
#include "random_engine.h"
#include "logger.h"
#include "../common.h"

#include <iostream>
#include <fstream>
#include <random>
#include <set>

namespace core {
	void GenerateInputFile(InputFileModel* model) {
		LOG(L"Generating input file...");

		//parse model
		int procCount = _STD stoi(model->proc_count);

		int fcfsCount = _STD stoi(model->fcfs_count);
		int sjfCount = _STD stoi(model->sjf_count);
		int rrCount = _STD stoi(model->rr_count);
		int edfCount = _STD stoi(model->edf_count);

		int rrSlice = _STD stoi(model->rr_slice);
		int rtf = _STD stoi(model->rtf);
		int maxw = _STD stoi(model->maxw);
		int stl = _STD stoi(model->stl);
		int fork = _STD stoi(model->fork);

#ifdef OVERRIDE_OVERHEAT_DELAY
		int overheat = OVERHEAT_DELAY;
#else
		int overheat = _STD stoi(model->overheat);
#endif

		//output
		_STD ofstream file(model->filename);

		//same format as deserializer

		file << fcfsCount << '\t'
			<< sjfCount << '\t'
			<< rrCount;

		if (model->enable_edf) {
			file << '\t' << edfCount;
		}

		file << '\n';

		file << rrSlice << '\n';

		file << rtf << '\t'
			<< maxw << '\t'
			<< stl << '\t'
			<< fork << '\t'
			<< overheat << '\n';

		file << procCount << '\n';

		//base values
		int pid = 1;
		int at = 1;

		for (int i = 0; i < procCount; i++) {
			//increment at by a random val
			at += RandomEngine::GetInt(0, 2);

			int procAt = at;
			int procPid = pid++;

			int procCpuTime = RandomEngine::GetInt(5, 30);

			int procDeadline = procAt + RandomEngine::GetInt(5, procCpuTime);

			int ioCount = RandomEngine::GetInt(0, 10);

			file << procAt << '\t'
				<< procPid << '\t'
				<< procCpuTime << '\t';

			if (model->enable_edf) {
				file << procDeadline << '\t';
			}

			file << ioCount << '\t';

			int ioBuf = 0;
			for (int j = 0; j < ioCount; j++) {
				ioBuf += RandomEngine::GetInt(0, procCpuTime);
				int ioR = ioBuf;
				int ioD = RandomEngine::GetInt(1, 10);

				char buf[50];
				sprintf(buf, "(%d,%d)%s", ioR, ioD, j < ioCount - 1 ? "," : "");

				file << buf;
			}

			file << '\n';
		}

		//sig kills
		if (model->generate_sigkills) {
			int time = 0; //kill time base
			_STD set<int> pids;
			while (true) {
				//time increments max is max arrival time / 10
				time = time + RandomEngine::GetInt(0, at / 10);
				if (time >= at || pids.size() >= procCount) break;

				//choose pid to be killed
				int killedPid;
				do {
					killedPid = RandomEngine::GetInt(1, pid - 1);
				} while (pids.contains(killedPid));

				pids.insert(killedPid);

				file << time << '\t'
					<< killedPid << '\n';
			}
		}

		file.close();

		LOG(L"Done");
	}
}