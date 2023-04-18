#pragma once

#define VEC_INT_X(v) ((int)v.x)
#define VEC_INT_Y(v) ((int)v.y)

namespace utils {
	/// <summary>
	/// Represents a 2D vector
	/// </summary>
	struct Vector2 {
		/// <summary>
		/// X coordinate
		/// </summary>
		float x;

		/// <summary>
		/// Y coordinate
		/// </summary>
		float y;

		/// <summary>
		/// Initialize the vector with 0
		/// </summary>
		Vector2();

		/// <summary>
		/// Initialize the vector with the provided values
		/// </summary>
		Vector2(float x, float y);

		/// <summary>
		/// Initialize the vector with the provided values
		/// </summary>
		Vector2(int x, int y);

		/// <summary>
		/// Returns the magnitude of the vector
		/// </summary>
		float magnitude();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rhs"></param>
		/// <returns></returns>
		Vector2 operator -(const Vector2& rhs);
	};
}