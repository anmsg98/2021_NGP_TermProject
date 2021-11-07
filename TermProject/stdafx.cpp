#include "stdafx.h"

float RandF(float Min, float Max)
{
	return Min + ((float)rand() / (float)RAND_MAX) * (Max - Min);
}

void DrawRect(HDC hMemDC, const POSITION& Position, float Width, float Height, HDC hMemDC2, const USER_RECT& Rect, COLORREF TransparentColor)
{
	int OriginX{ (int)(Position.m_X - 0.5f * Width) };
	int OriginY{ (int)(Position.m_Y - 0.5f * Height) };

	TransparentBlt(hMemDC, OriginX, OriginY, (int)Width, (int)Height,
				  hMemDC2, (int)Rect.m_Left, (int)Rect.m_Top, (int)Rect.m_Width, (int)Rect.m_Height, TransparentColor);
}
