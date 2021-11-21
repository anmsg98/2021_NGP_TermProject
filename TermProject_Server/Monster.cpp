#include "stdafx.h"
#include "Monster.h"
int KillCount = 0;
void CMonster::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		if (m_Hp > 0.0f)
		{
			if (m_IsCollided)
			{
				m_CollisionDuration += DeltaTime;

				if (m_CollisionDuration < 0.2f)
				{
					m_Position.m_X += (GetDirection().m_X / GetLength()) * DeltaTime * 300.0f;
					m_Position.m_Y += (GetDirection().m_Y / GetLength()) * DeltaTime * 300.0f;
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
				m_Position.m_X += (GetDirection().m_X / GetLength()) * DeltaTime * 100.0f;
				m_Position.m_Y += (GetDirection().m_Y / GetLength()) * DeltaTime * 100.0f;
			}
		}
		else
		{
			const int FrameFPS{ 15 };

			m_AnimationTime += FrameFPS * DeltaTime;

			if (m_AnimationTime >= m_AnimationFrame)
			{
				SetDirection(0, 0);
				m_IsActive = false;
				m_AnimationTime = 0.0f;
				KillCount++;
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
