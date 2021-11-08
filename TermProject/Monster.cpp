#include "stdafx.h"
#include "Monster.h"

void CMonster::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		if (m_Hp != 0)
		{
			m_Position.m_X += (GetDirect().x / GetLength()) * DeltaTime * 100.0f;
			m_Position.m_Y += (GetDirect().y / GetLength()) * DeltaTime * 100.0f;
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

		Rect.m_Left = Rect.m_Width * FrameIndex;

		DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, Rect, CFileManager::GetInstance()->GetTransparentColor());

#ifdef DEBUG_HP
		TCHAR HpText[64]{};

		wsprintf(HpText, _T("%d"), m_Hp);
		TextOut(hMemDC, (int)(m_Position.m_X - 10.0f), (int)(m_Position.m_Y - 0.5f * m_Height), HpText, lstrlen(HpText));
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

void CMonster::SetDirect(float DirX, float DirY)
{
	m_Direction.x = (int)DirX;
	m_Direction.y = (int)DirY;
}

POINT CMonster::GetDirect() const
{
	return m_Direction;
}
