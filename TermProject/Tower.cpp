#include "stdafx.h"
#include "Tower.h"

void CTower::Animate()
{

}

void CTower::Render(HDC hMemDC, HDC hMemDC2)
{
	if (m_IsActive)
	{
		USER_RECT BitmapRect{ CFileManager::GetInstance()->GetRect("TOWER") };
		
		if (m_Hp <= 0.0f)
		{
			int FrameIndex{ (int)m_AnimationTime % m_AnimationFrame };
			
			BitmapRect.m_Left = BitmapRect.m_Width * FrameIndex;
		}

		DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2,BitmapRect, CFileManager::GetInstance()->GetTransparentColor());

		// Ã¼·Â¹Ù
		VECTOR2D Position{ GetPosition() };

		Position.m_Y -= 0.6f * BitmapRect.m_Height;
		BitmapRect = CFileManager::GetInstance()->GetRect("HP_1");

		DrawRect(hMemDC, Position, (float)BitmapRect.m_Width, (float)BitmapRect.m_Height, hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());

		BitmapRect = CFileManager::GetInstance()->GetRect("HP_2");

		float CurrentWidth{ BitmapRect.m_Width * (m_Hp / m_MaxHp) };

		FixedDrawRect(hMemDC, Position, (float)BitmapRect.m_Width, (float)BitmapRect.m_Height, CurrentWidth, (float)BitmapRect.m_Height, hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());

#ifdef DEBUG_HP
		TCHAR HpText[32]{};

		sprintf(HpText, "%.f", m_Hp);
		TextOut(hMemDC, (int)(m_Position.m_X - 15.0f), (int)(m_Position.m_Y - 0.5f * m_Size.m_Y), HpText, lstrlen(HpText));
#endif
	}
}
