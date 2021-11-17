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
