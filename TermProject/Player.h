#pragma once
#include "GameObject.h"

#define MAX_BULLET 10

// ============================================== CBullet ==============================================

class CBullet : public CGameObject
{
public:
	CBullet() = default;
	~CBullet() = default;

	virtual void Update(float DeltaTime);
};

// ============================================== CPlayer ============================================== 

class CPlayer : public CGameObject
{
private:
	POINT				m_CameraStartPosition{};

	CBullet* 			m_Bullets;
	int					m_BulletIndex{};

public:
	CPlayer();
	~CPlayer();

	const POINT& GetCameraStartPosition() const;
	CBullet* GetBullets();

	void FireBullet();

	void UpdateCamera(const RECT& ClientRect, const RECT& MapRect);
	virtual void Animate(float DeltaTime);
};
