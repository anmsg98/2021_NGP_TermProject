#pragma once
#include "GameObject.h"

class CMonster : public CGameObject
{
private:
	int m_Type{};
	float m_LineLength{};
	POINT m_Dir{};

public:
	CMonster() = default;
	~CMonster() = default;

	void SetType(int Type);
	int  GetType() const;

	void SetLength(float length) { m_LineLength = length; }
	float GetLength() { return m_LineLength; }

	void SetDirect(float dirx, float diry) { m_Dir.x = dirx, m_Dir.y = diry; }
	POINT GetDirect() { return m_Dir; }

	virtual void Animate(float DeltaTime);
	//virtual void Render();

};

