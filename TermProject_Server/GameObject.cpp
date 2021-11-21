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

void CGameObject::SetMaxHp(float Hp)
{
	m_MaxHp = Hp;
}

float CGameObject::GetMaxHp() const
{
	return m_MaxHp;
}

void CGameObject::SetHp(float Hp)
{
	m_Hp = Hp;

	if (m_Hp <= 0.0f)
	{
		m_Hp = 0.0f;
	}
	else if (m_Hp >= m_MaxHp)
	{
		m_Hp = m_MaxHp;
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

void CGameObject::SetSize(const VECTOR2D& Size)
{
	m_Size = Size;
}

const VECTOR2D& CGameObject::GetSize() const
{
	return m_Size;
}

void CGameObject::SetWidth(float Width)
{
	m_Size.m_X = Width;
}

float CGameObject::GetWidth() const
{
	return m_Size.m_X;
}

void CGameObject::SetHeight(float Height)
{
	m_Size.m_Y = Height;
}

float CGameObject::GetHeight() const
{
	return m_Size.m_Y;
}
