#pragma once
#include "GameObject.h"

class CTower : public CGameObject
{
private:
	const int				m_AnimationFrame{ 6 };
	float					m_AnimationTime{};

public:
	CTower() = default;
	~CTower() = default;

	void Animate(float DeltaTime);
	void Render(HDC hMemDC, HDC hMemDC2);
};
