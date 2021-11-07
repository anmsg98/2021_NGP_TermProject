#pragma once
#include "GameObject.h"

#define MAX_BULLET 10

// ============================================== CBullet ==============================================

class CBullet : public CGameObject
{
private:
	float m_LineLength{};
	POINT m_Dir{};
public:
	CBullet() = default;
	~CBullet() = default;
	virtual void Update(float DeltaTime);
	void SetLength(float length) { m_LineLength = length; }
	float GetLength() { return m_LineLength; }
	void SetDirect(float dirx, float diry) { m_Dir.x = dirx, m_Dir.y = diry; }
	POINT GetDirect() { return m_Dir; }
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

	void FireBullet(int mx, int my);

	void UpdateCamera(const RECT& ClientRect, const RECT& MapRect);
	virtual void Animate(float DeltaTime);
};
