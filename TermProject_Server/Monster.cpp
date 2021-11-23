#include "stdafx.h"
#include "Monster.h"

void CMonster::Animate()
{
	if (m_IsActive)
	{
		if (m_Hp > 0.0f)
		{
			if (m_IsCollided)
			{
				m_CollisionDuration += 1.0f;

				if (m_CollisionDuration < 10.0f)
				{
					m_Position.m_X += (GetDirection().m_X / GetLength()) * 3.0f;
					m_Position.m_Y += (GetDirection().m_Y / GetLength()) * 3.0f;
				}
				else
				{
					m_IsCollided = false;
					m_CollisionDuration = 0.0f;

					SetDirection(m_PrevDirection.m_X, m_PrevDirection.m_Y);
					SetLength(sqrtf(powf((float)GetDirection().m_X, 2) + powf((float)GetDirection().m_Y, 2)));
				}
			}
			else
			{
				m_Position.m_X += (GetDirection().m_X / GetLength());
				m_Position.m_Y += (GetDirection().m_Y / GetLength());
			}
		}
		else
		{
			m_AnimationTime += 0.25f;

			if (m_AnimationTime >= m_AnimationFrame)
			{
				m_IsActive = false;
				m_IsCollided = false;
				m_AnimationTime = 0.0f;
				m_CollisionDuration = 0.0f;
			}
		}
	}
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
