#pragma once
#include "GameObject.h"

#define MAX_ITEM 5

class CItem : public CGameObject
{
public:
	enum { ATTACK_POWER_UP = 1, HP_UP };

private:
	int				m_Type{};

public:
	CItem() = default;
	~CItem() = default;

	void Animate();
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetType(int Type);
	int GetType() const;
};
