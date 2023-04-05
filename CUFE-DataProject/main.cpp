#include <iostream>
#include <Windows.h>

#include "common.h"
#include "core/application.h"

using namespace core;

int main() {
	Application app;

	//initialize app
	app.Initialize(APP_NAME, _UTIL Vector2(APP_SIZE_WIDTH, APP_SIZE_HEIGHT));

	//main loop
	while (true) {
		//update app
		app.Update();

		//sleep for 10ms
		Sleep(10);
	}

	return 0;
}