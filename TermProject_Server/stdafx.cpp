#include "stdafx.h"

float RandF(float Min, float Max)
{
	return Min + ((float)rand() / (float)RAND_MAX) * (Max - Min);
}

namespace Vector
{
	float Distance(const VECTOR2D& Position1, const VECTOR2D& Position2)
	{
		return sqrtf(powf(Position2.m_X - Position1.m_X, 2) + powf(Position2.m_Y - Position1.m_Y, 2));
	}

	float Length(const VECTOR2D& Vector)
	{
		return sqrtf(powf(Vector.m_X, 2) + powf(Vector.m_Y, 2));
	}

	VECTOR2D Inverse(const VECTOR2D& Vector)
	{
		return VECTOR2D(-Vector.m_X, -Vector.m_Y);
	}
}
