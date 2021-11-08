#include "stdafx.h"
#include "Tower.h"

void CTower::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		if (m_Hp <= 0.0f)
		{
			const int FrameFPS{ 7 };

			m_AnimationTime += FrameFPS * DeltaTime;

			if (m_AnimationTime >= m_AnimationFrame)
			{
				m_AnimationTime = 0.0f;
				m_IsActive = false;
			}
		}
	}
}

void CTower::Render(HDC hMemDC, HDC hMemDC2)
{
	if (m_IsActive)
	{
		USER_RECT Rect{ CFileManager::GetInstance()->GetRect("Tower_1_1") };
		int FrameIndex{ (int)m_AnimationTime % m_AnimationFrame };

		Rect.m_Left = Rect.m_Width * FrameIndex;

		DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, Rect, CFileManager::GetInstance()->GetTransparentColor());

#ifdef DEBUG_HP
		TCHAR HpText[64]{};

		wsprintf(HpText, _T("%d"), m_Hp);
		TextOut(hMemDC, (int)(m_Position.m_X - 15.0f), (int)(m_Position.m_Y - 0.5f * m_Height), HpText, lstrlen(HpText));
#endif
	}
}
