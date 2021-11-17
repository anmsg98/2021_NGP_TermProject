#include "stdafx.h"
#include "GameObject.h"

void CGameObject::Animate(float DeltaTime)
{

}

void CGameObject::Render(HDC hMemDC, HDC hMemDC2)
{

}

void CGameObject::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

bool CGameObject::IsActive() const
{
	return m_IsActive;
}

void CGameObject::SetHp(float Hp)
{
	m_Hp = Hp;

	if (m_Hp <= 0.0f)
	{
		m_Hp = 0.0f;
	}
}

float CGameObject::GetHp() const
{
	return m_Hp;
}

void CGameObject::SetPosition(const POSITION& Position)
{
	m_Position = Position;
}

void CGameObject::SetPosition(float Xpos, float Ypos)
{
	SetPosition(POSITION(Xpos, Ypos));
}

const POSITION& CGameObject::GetPosition() const
{
	return m_Position;
}

void CGameObject::SetBitmapRect(const USER_RECT& Rect)
{
	m_BitmapRect = Rect;
}

const USER_RECT& CGameObject::GetBitmapRect() const
{
	return m_BitmapRect;
}

void CGameObject::SetWidth(float Width)
{
	m_Width = Width;
}

float CGameObject::GetWidth() const
{
	return m_Width;
}

void CGameObject::SetHeight(float Height)
{
	m_Height = Height;
}

float CGameObject::GetHeight() const
{
	return m_Height;
}
