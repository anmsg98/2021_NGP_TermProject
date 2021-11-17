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

}
