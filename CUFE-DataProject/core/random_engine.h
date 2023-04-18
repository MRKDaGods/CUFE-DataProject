#pragma once

#include <random>

namespace core {
	/// <summary>
	/// Random engine based on the mersenne twister engine
	/// </summary>
	class RandomEngine {
	private:
		static _STD mt19937* ms_Generator;

	public:
		/// <summary>
		/// Initializes the random engine
		/// </summary>
		static void Initialize();

		/// <summary>
		/// Returns a random between min and max inclusive
		/// </summary>
		static int GetInt(int min, int max);

		/// <summary>
		/// Cleans up the engine
		/// </summary>
		static void Clean();
	};
}