#pragma once

#include "../common.h"

#include <mutex>
#include <functional>

namespace utils {
	// Provides a simple lock interface
	class Lock {
	private:
		// The mutex
		_STD mutex m_Mutex;

		// Depth of lock
		int m_Depth;

		// Are we locked?
		bool m_Locked;

	public:
		Lock();

		// Acquires the mutex
		void Acquire();

		// Releases the mutex
		void Release();

		// Executes func in an interlocked context
		void Interlocked(_STD function<void()> func);
	};
}