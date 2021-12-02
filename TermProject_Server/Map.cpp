#include "stdafx.h"
#include "Map.h"

void CMap::Render(HDC hMemDC, HDC hMemDC2)
{

}

void CMap::SetRect(const RECT& Rect)
{
	m_Rect = Rect;
}

const RECT& CMap::GetRect() const
{
	return m_Rect;
}
