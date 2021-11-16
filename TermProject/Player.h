#pragma once
#include "GameObject.h"

#define MAX_PLAYER 4
#define MAX_BULLET 10

// ============================================== CBullet ==============================================

class CBullet : public CGameObject
{
private:
	float			m_AttackPower{ 10.0 };

	float			m_Length{};
	POINT			m_Direction{};

public:
	CBullet() = default;
	~CBullet() = default;
 
	void Animate(float DeltaTime);
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetAttackPower(float AttackPower);
	float GetAttackPower() const;

	void SetLength(float Length);
	float GetLength() const;

	void SetDirection(float DirX, float DirY);
	POINT GetDirection() const;
};

// ============================================== CPlayer ============================================== 

class CPlayer : public CGameObject
{
private:
	int					m_ID{};

	SOCKET			    m_Socket{};
	SOCKADDR_IN		    m_SocketAddress{};

	POINT				m_CameraStartPosition{};

	POINT			    m_Direction{};

	bool				m_IsGetItem{};
	float				m_ItemDuration{};

	CBullet			    m_Bullets[MAX_BULLET]{};
	int					m_BulletIndex{};

public:
	CPlayer();
	~CPlayer() = default;

	void Animate(float DeltaTime);
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetID(int ID);
	int GetID() const;

	void SetSocket(SOCKET Socket);
	SOCKET GetSocket() const;

	void SetSocketAddress(const SOCKADDR_IN& SocketAddress);
	const SOCKADDR_IN& GetSocketAddress() const;

	const POINT& GetCameraStartPosition() const;

	void SetDirection(float DirX, float DirY);
	POINT GetDirection() const;

	bool IsGetItem() const;
	void ReinforceBullet();

	void FireBullet(const POINT& CursorPos);

	void UpdateCamera(const RECT& ClientRect, const RECT& MapRect);

};
