#include "stdafx.h"
#include "Monster.h"


void CMonster::SetType(int Type)
{
	m_Type = Type;
}

int CMonster::GetType() const
{
	return m_Type;
}

void CMonster::Animate(float DeltaTime)
{
	m_Position.m_X += (GetDirect().x / GetLength()) * DeltaTime * 100.f;
	m_Position.m_Y += (GetDirect().y / GetLength()) * DeltaTime * 100.f;

	
}