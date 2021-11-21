#include "stdafx.h"
#include "Map.h"
#include "FileManager.h"

void CMap::Render(HDC hMemDC, HDC hMemDC2)
{
	USER_RECT BitmapRect{ CFileManager::GetInstance()->GetRect("BACKGROUND") };

	BitBlt(hMemDC, BitmapRect.m_Left, BitmapRect.m_Top, BitmapRect.m_Width, BitmapRect.m_Height, hMemDC2, 0, 0, SRCCOPY);
}

void CMap::SetRect(const RECT& Rect)
{
	m_Rect = Rect;
}

const RECT& CMap::GetRect() const
{
	return m_Rect;
}
