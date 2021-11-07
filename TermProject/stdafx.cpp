#include "stdafx.h"

void DrawRectangle(const HDC& hDC, const POSITION& Position, float Width, float Height)
{
	RECT Rect{};

	Rect.left = Position.m_X - 0.5f * Width;
	Rect.top = Position.m_Y - 0.5f * Height;
	Rect.right = Rect.left + Width;
	Rect.bottom = Rect.top + Height;

	Rectangle(hDC, Rect.left, Rect.top, Rect.right, Rect.bottom);
}


float RandF2(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}