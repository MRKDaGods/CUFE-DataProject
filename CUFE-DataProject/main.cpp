#include <iostream>
#include <Windows.h>

#include "common.h"
#include "core/scheduler.h"

using namespace core;

int main() {
	Scheduler sched;

	while (true) {
		Sleep(10);
	}

	return 0;
}