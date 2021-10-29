#include "stdafx.h"
#include "GameObject.h"

CGameObject::CGameObject()
{

}

CGameObject::~CGameObject()
{

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
