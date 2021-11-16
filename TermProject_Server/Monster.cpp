#include "stdafx.h"
#include "Monster.h"

void CMonster::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		if (m_Hp != 0)
		{
			if (is_TowerCollide)
			{
				m_TimeElapsed_T += DeltaTime;
				if (m_TimeElapsed_T < 0.3f)
				{
					m_Position.m_X -= (GetDirection().x / GetLength()) * DeltaTime * 300.0f;
					m_Position.m_Y -= (GetDirection().y / GetLength()) * DeltaTime * 300.0f;
				}
				else
				{
					m_TimeElapsed_T = 0.0f;
					is_TowerCollide = false;
				}
			}
			if (is_BulletCollide)
			{
				m_TimeElapsed_B += DeltaTime;
				if (m_TimeElapsed_B < 0.3f)
				{
					m_Position.m_X += (GetDirection().x / GetLength()) * DeltaTime * 300.0f;
					m_Position.m_Y += (GetDirection().y / GetLength()) * DeltaTime * 300.0f;
				}
				else
				{
					SetDirection(m_MapPosition.x - GetPosition().m_X, m_MapPosition.y - GetPosition().m_Y);
					SetLength(sqrtf(powf((float)GetDirection().x, 2) + powf((float)GetDirection().y, 2)));
					m_TimeElapsed_B = 0.0f;
					is_BulletCollide = false;
				}
			}

			else
			{
				m_Position.m_X += (GetDirection().x / GetLength()) * DeltaTime * 100.0f;
				m_Position.m_Y += (GetDirection().y / GetLength()) * DeltaTime * 100.0f;
			}
		}
		else
		{
			const int FrameFPS{ 15 };

			m_AnimationTime += FrameFPS * DeltaTime;

			if (m_AnimationTime >= m_AnimationFrame)
			{
				m_AnimationTime = 0.0f;
				m_IsActive = false;
			}
		}
	}
}

void CMonster::Render(HDC hMemDC, HDC hMemDC2)
{
	if (m_IsActive)
	{
		USER_RECT Rect{};
		int FrameIndex{ (int)m_AnimationTime % m_AnimationFrame };

		if (m_Type == LOWER)
		{
			Rect = CFileManager::GetInstance()->GetRect("Monster_1_1");
		}
		else if (m_Type == MIDDLE)
		{
			Rect = CFileManager::GetInstance()->GetRect("Monster_2_1");
		}
		else if (m_Type == UPPER)
		{
			Rect = CFileManager::GetInstance()->GetRect("Monster_3_1");
		}

		Rect.m_Left = Rect.m_Width * FrameIndex;

		DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, Rect, CFileManager::GetInstance()->GetTransparentColor());

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

void CMonster::SetDirection(float DirX, float DirY)
{
	m_Direction.x = (int)DirX;
	m_Direction.y = (int)DirY;
}

POINT CMonster::GetDirection() const
{
	return m_Direction;
}

POINT CMonster::GetMapPosition() const
{
	return m_MapPosition;
}