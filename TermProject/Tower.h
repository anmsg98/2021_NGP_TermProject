#pragma once
#include "GameObject.h"

class CTower : public CGameObject
{
private:
	const int				m_AnimationFrame{ 6 };
	float					m_AnimationTime{};

public:
	CTower() = default;
	virtual ~CTower() = default;

	virtual void Animate(float DeltaTime);
	virtual void Render(HDC hMemDC, HDC hMemDC2);
};
