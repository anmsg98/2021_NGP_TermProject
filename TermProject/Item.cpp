#include "stdafx.h"
#include "Item.h"

void CItem::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		m_Hp -= DeltaTime;

		cout << m_Hp << ", " << DeltaTime << endl;

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
		USER_RECT Rect{};

		if (m_Type == HP_UP)
		{
			Rect = CFileManager::GetInstance()->GetRect("Potion_1");
		}
		else if (m_Type == ATTACK_POWER_UP)
		{
			Rect = CFileManager::GetInstance()->GetRect("Potion_2");
		}

		DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, Rect, CFileManager::GetInstance()->GetTransparentColor());
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
