#include "random_engine.h"

namespace core {
	_STD mt19937* RandomEngine::ms_Generator = 0;

	void RandomEngine::Initialize() {
		//create device
		_STD random_device device;

		//init gen
		ms_Generator = new _STD mt19937(device());
	}

	int RandomEngine::GetInt(int min, int max) {
		//create dist and use it
		_STD uniform_int_distribution<> distribution(min, max);
		return distribution(*ms_Generator);
	}

	void RandomEngine::Clean() {
		if (ms_Generator != 0) {
			delete ms_Generator;
		}
	}
}