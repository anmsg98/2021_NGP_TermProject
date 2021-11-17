#include "stdafx.h"
#include "Item.h"

void CItem::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		m_Hp -= DeltaTime;

		if (m_Hp <= 0.0f)
		{
			m_IsActive = false;
		}
	}
}

void CItem::Render(HDC hMemDC, HDC hMemDC2)
{
	if (m_IsActive)
	{
		DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, m_BitmapRect, CFileManager::GetInstance()->GetTransparentColor());
	}
}

void CItem::SetType(int Type)
{
	m_Type = Type;
}

int CItem::GetType() const
{
	return m_Type;
}
