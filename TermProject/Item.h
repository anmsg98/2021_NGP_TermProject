#pragma once
#include "GameObject.h"

class CItem : public CGameObject
{
public:
	enum { ATTACK_POWER_UP = 1, HP_UP };

private:
	int				m_Type{};

public:
	CItem() = default;
	virtual ~CItem() = default;

	virtual void Animate(float DeltaTime);
	virtual void Render(HDC hMemDC, HDC hMemDC2);

	void SetType(int Type);
	int GetType() const;
};
