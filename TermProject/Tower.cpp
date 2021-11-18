#include "stdafx.h"
#include "Tower.h"

void CTower::Animate(float DeltaTime)
{

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
