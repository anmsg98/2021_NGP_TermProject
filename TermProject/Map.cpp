#include "stdafx.h"
#include "Map.h"
#include "FileManager.h"

void CMap::Render(HDC hMemDC, HDC hMemDC2)
{
	BitBlt(hMemDC, m_BitmapRect.m_Left, m_BitmapRect.m_Top, m_BitmapRect.m_Width, m_BitmapRect.m_Height, hMemDC2, 0, 0, SRCCOPY);
}

void CMap::SetRect(const RECT& Rect)
{
	m_Rect = Rect;
}

const RECT& CMap::GetRect() const
{
	return m_Rect;
}

void CMap::SetBitmapRect(const USER_RECT& Rect)
{
	m_BitmapRect = Rect;
}

const USER_RECT& CMap::GetBitmapRect() const
{
	return m_BitmapRect;
}
