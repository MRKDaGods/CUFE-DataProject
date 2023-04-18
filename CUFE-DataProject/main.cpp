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

	while (true) {
		if (sched.GetSimulationInfo()->CanUpdateScheduler()) {
			sched.Update();
		}

		Sleep(10);
	}

	//clean up random engine
	RandomEngine::Clean();

	return 0;
}