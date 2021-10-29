#pragma once
#include "GameObject.h"

class CPlayer : public CGameObject
{
private:
	POSITION			m_CameraStartPosition{};

public:
	CPlayer() = default;
	~CPlayer() = default;

	const POSITION& GetCameraStartPosition() const;

	void Update(const RECT& ClientRect, const RECT& MapRect);
};
