#pragma once
#include "GameObject.h"

class CMonster : public CGameObject
{
private:
	int				m_Type{};

	const int		m_AnimationFrame{ 5 };
	float			m_AnimationTime{};

	float			m_Length{};
	POINT			m_Direction{};

public:
	CMonster() = default;
	~CMonster() = default;

	virtual void Animate(float DeltaTime);
	virtual void Render(HDC hMemDC, HDC hMemDC2);

	void SetType(int Type);
	int GetType() const;

	void SetLength(float Length);
	float GetLength() const;

	void SetDirect(float DirX, float DirY);
	POINT GetDirect() const;
};
