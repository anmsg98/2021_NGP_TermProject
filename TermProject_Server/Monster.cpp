#include "stdafx.h"
#include "Monster.h"

void CMonster::Animate(float DeltaTime)
{

}

void CMonster::Render(HDC hMemDC, HDC hMemDC2)
{

}

void CMonster::SetType(int Type)
{
	m_Type = Type;
}

int CMonster::GetType() const
{
	return m_Type;
}

void CMonster::SetLength(float Length)
{
	m_Length = Length;
}

float CMonster::GetLength() const
{
	return m_Length;
}

void CMonster::SetDirect(float DirX, float DirY)
{
	m_Direction.x = (int)DirX;
	m_Direction.y = (int)DirY;
}

POINT CMonster::GetDirect() const
{
	return m_Direction;
}
