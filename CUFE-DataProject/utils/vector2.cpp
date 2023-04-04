#include "vector2.h"

#include <math.h>

namespace utils {
	Vector2::Vector2() : Vector2(0.f, 0.f)
	{
	}

	Vector2::Vector2(float x, float y) : x(x), y(y)
	{
	}

	float Vector2::magnitude()
	{
		return sqrtf(x * x + y * y);
	}

	Vector2 Vector2::operator-(const Vector2& rhs)
	{
		return Vector2(x - rhs.x, y - rhs.y);
	}
}