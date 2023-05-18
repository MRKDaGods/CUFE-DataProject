#include <iostream>
#include <Windows.h>

#include "common.h"
#include "core/scheduler.h"
#include "core/random_engine.h"

using namespace core;

int main() {
	//init random engine
	RandomEngine::Initialize();

	Scheduler sched;

	LOG(L"Initializing...");

	while (true) {
		int sleepTime = 10; //by default sleep for 10ms

		if (sched.GetSimulationInfo()->CanUpdateScheduler(&sleepTime)) {
			sched.Update();
		}

		Sleep(sleepTime);
	}

	//clean up random engine
	RandomEngine::Clean();

	return 0;
}