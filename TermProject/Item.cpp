#include "stdafx.h"
#include "Item.h"

void CItem::Animate()
{

}

void CItem::Render(HDC hMemDC, HDC hMemDC2)
{
	if (m_IsActive)
	{
		USER_RECT BitmapRect{};

		switch (m_Type)
		{
		case HP_UP:
			BitmapRect = CFileManager::GetInstance()->GetRect("POTION_1");
			break;
		case ATTACK_POWER_UP:
			BitmapRect = CFileManager::GetInstance()->GetRect("POTION_2");
			break;
		}

		DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());
		
		if (m_Sounded)
		{
			CSoundManager::GetInstance()->Play(CSoundManager::ITEM_DROP_SOUND, 0.5f);
			m_Sounded = false;
		}
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
