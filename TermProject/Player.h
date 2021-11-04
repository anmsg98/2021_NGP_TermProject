#pragma once
#include "GameObject.h"

class CPlayer : public CGameObject
{
private:
	POINT			m_CameraStartPosition{};

public:
	CPlayer();
	~CPlayer();
	CBulletObject* m_ppBullets[30];

	const POINT& GetCameraStartPosition() const;
	void FireBullet();
	void Update(const RECT& ClientRect, const RECT& MapRect);
};
