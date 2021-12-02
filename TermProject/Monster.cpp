#include "stdafx.h"
#include "Monster.h"

void CMonster::Animate()
{

}

void CMonster::Render(HDC hMemDC, HDC hMemDC2)
{
	if (m_IsActive)
	{
		USER_RECT BitmapRect{};

		switch (m_Type)
		{
		case LOWER:
			BitmapRect = CFileManager::GetInstance()->GetRect("MONSTER_1");
			break;
		case MIDDLE:
			BitmapRect = CFileManager::GetInstance()->GetRect("MONSTER_2");
			break;
		case UPPER:
			BitmapRect = CFileManager::GetInstance()->GetRect("MONSTER_3");
			break;
		}

		if (m_Hp <= 0.0f)
		{
			if (m_Sounded)
			{
				CSoundManager::GetInstance()->Play(CSoundManager::MONSTER_DEAD_SOUND, 0.4f);
				m_Sounded = false;
			}

			int FrameIndex{ (int)m_AnimationTime % m_AnimationFrame };

			BitmapRect.m_Left = BitmapRect.m_Width * FrameIndex;
		}

		DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());

		// Ã¼·Â¹Ù
		VECTOR2D Position{ GetPosition() };

		Position.m_Y -= 0.5f * BitmapRect.m_Height;
		BitmapRect = CFileManager::GetInstance()->GetRect("HP_1");

		DrawRect(hMemDC, Position, (float)BitmapRect.m_Width, (float)BitmapRect.m_Height, hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());

		BitmapRect = CFileManager::GetInstance()->GetRect("HP_2");

		float CurrentWidth{ BitmapRect.m_Width * (m_Hp / m_MaxHp) };

		FixedDrawRect(hMemDC, Position, (float)BitmapRect.m_Width, (float)BitmapRect.m_Height, CurrentWidth, (float)BitmapRect.m_Height, hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());

#ifdef DEBUG_PRINT_HP
		TCHAR HpText[32]{};

		sprintf(HpText, "%.f", m_Hp);
		TextOut(hMemDC, (int)(m_Position.m_X - 15.0f), (int)(m_Position.m_Y - 0.5f * m_Size.m_Y), HpText, lstrlen(HpText));
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
