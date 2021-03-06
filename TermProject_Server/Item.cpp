#include "stdafx.h"
#include "Item.h"

void CItem::Animate()
{
	if (m_IsActive)
	{
		m_Hp -= 1.0f;

		if (m_Hp <= 0.0f)
		{
			m_IsActive = false;
		}
	}
	else
	{
		if (m_Sounded)
		{
			m_Sounded = false;
		}
	}
}

void CItem::Render(HDC hMemDC, HDC hMemDC2)
{

}

void CItem::SetType(int Type)
{
	m_Type = Type;
}

int CItem::GetType() const
{
	return m_Type;
}
