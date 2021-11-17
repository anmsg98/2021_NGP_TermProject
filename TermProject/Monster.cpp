#include "stdafx.h"
#include "Monster.h"

void CMonster::Animate(float DeltaTime)
{

}

void CMonster::Render(HDC hMemDC, HDC hMemDC2)
{
	if (m_IsActive)
	{
		int FrameIndex{ (int)m_AnimationTime % m_AnimationFrame };

		m_BitmapRect.m_Left = m_BitmapRect.m_Width * FrameIndex;

		DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, m_BitmapRect, CFileManager::GetInstance()->GetTransparentColor());

#ifdef DEBUG_HP
		TCHAR HpText[32]{};

		sprintf(HpText, "%.f", m_Hp);
		TextOut(hMemDC, (int)(m_Position.m_X - 15.0f), (int)(m_Position.m_Y - 0.5f * m_Height), HpText, lstrlen(HpText));
#endif
	}
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

void CMonster::SetDirection(const VECTOR2D& Direction)
{
	m_Direction = Direction;
}

void CMonster::SetDirection(float DirX, float DirY)
{
	SetDirection(VECTOR2D(DirX, DirY));
}

VECTOR2D CMonster::GetDirection() const
{
	return m_Direction;
}

void CMonster::SetPrevDirection(const VECTOR2D& Direction)
{
	m_PrevDirection = Direction;
}

void CMonster::SetPrevDirection(float DirX, float DirY)
{
	SetPrevDirection(VECTOR2D(DirX, DirY));
}

VECTOR2D CMonster::GetPrevDirection() const
{
	return m_PrevDirection;
}

bool CMonster::IsCollided() const
{
	return m_IsCollided;
}

void CMonster::PrepareCollision()
{
	m_IsCollided = true;
	m_CollisionDuration = 0.0f;
}
