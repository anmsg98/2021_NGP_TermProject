#include "stdafx.h"
#include "Monster.h"

void CMonster::Animate(float DeltaTime)
{
	m_Position.m_X += (GetDirect().x / GetLength()) * DeltaTime * 100.f;
	m_Position.m_Y += (GetDirect().y / GetLength()) * DeltaTime * 100.f;
}

void CMonster::Render(HDC hMemDC, HDC hMemDC2)
{
	USER_RECT Rect{};

	if (m_Type == 1)
	{
		Rect = CFileManager::GetInstance()->GetRect("Monster_1_1");
	}
	else if (m_Type == 2)
	{
		Rect = CFileManager::GetInstance()->GetRect("Monster_2_1");
	}
	else if (m_Type == 3)
	{
		Rect = CFileManager::GetInstance()->GetRect("Monster_3_1");
	}

	DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, Rect, CFileManager::GetInstance()->GetTransparentColor());
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
