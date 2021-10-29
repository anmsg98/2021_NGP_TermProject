#pragma once
#include "GameObject.h"

class CPlayer : public CGameObject
{
private:
	POINT			m_CameraStartPosition{};

public:
	CPlayer() = default;
	~CPlayer() = default;

	const POINT& GetCameraStartPosition() const;

	void Update(const RECT& ClientRect, const RECT& MapRect);
};
