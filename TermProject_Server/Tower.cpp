#include "stdafx.h"
#include "Tower.h"

void CTower::Animate()
{
	if (m_IsActive)
	{
		if (m_Hp <= 0.0f)
		{
			m_AnimationTime += 0.25f;

			if (m_AnimationTime >= m_AnimationFrame)
			{
				m_IsActive = false;
				m_AnimationTime = 0.0f;
			}
		}
	}
}

void CTower::Render(HDC hMemDC, HDC hMemDC2)
{

}
