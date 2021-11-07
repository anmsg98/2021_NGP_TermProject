#pragma once
#include "GameObject.h"

#define MAX_BULLET 10

// ============================================== CBullet ==============================================

class CBullet : public CGameObject
{
private:
	float			m_Length{};
	POINT			m_Direction{};

public:
	CBullet() = default;
	~CBullet() = default;

	virtual void Animate(float DeltaTime);
	virtual void Render(HDC hMemDC, HDC hMemDC2);

	void SetLength(float Length);
	float GetLength() const;

	void SetDirect(float DirX, float DirY);
	POINT GetDirect() const;
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
	
	virtual void Animate(float DeltaTime);
	virtual void Render(HDC hMemDC, HDC hMemDC2);

	const POINT& GetCameraStartPosition() const;
	CBullet* GetBullets();

	void FireBullet(const POINT& CursorPos);

	void UpdateCamera(const RECT& ClientRect, const RECT& MapRect);
};
