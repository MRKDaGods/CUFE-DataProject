#include "lock.h"

namespace utils {
	Lock::Lock() : m_Depth(0), m_Locked(false) {
	}

	void Lock::Acquire() {
		m_Mutex.lock();
	}

	void Lock::Release() {
		m_Mutex.unlock();
	}

	void Lock::Interlocked(_STD function<void()> func) {
		if (func == 0) return;

		Acquire();
		func();
		Release();
	}
}
