#include "stdafx.h"
#include "Map.h"
#include "FileManager.h"

void CMap::Render(HDC hMemDC, HDC hMemDC2)
{
	USER_RECT Rect{ CFileManager::GetInstance()->GetRect("Background") };

	BitBlt(hMemDC, Rect.m_Left, Rect.m_Top, Rect.m_Width, Rect.m_Height, hMemDC2, 0, 0, SRCCOPY);
}

void CMap::SetRect(const RECT& Rect)
{
	m_Rect = Rect;
}

const RECT& CMap::GetRect() const
{
	return m_Rect;
}
