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

			if (m_Sounded)
			{
				m_Sounded = false;
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

const VECTOR2D& CMonster::GetDirection() const
{
	return m_Direction;
}

bool CMonster::IsCollided() const
{
	return m_IsCollided;
}

void CMonster::SetCollision(bool IsCollided)
{
	m_IsCollided = IsCollided;
}
