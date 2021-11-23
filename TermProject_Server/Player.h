#pragma once
#include "GameObject.h"

#define MAX_PLAYER  2
#define MAX_BULLET  5

// ============================================== CBullet ==============================================

class CBullet : public CGameObject
{
private:
	bool				m_IsCollided{};

	float				m_AttackPower{ 10.0f };

	float				m_Length{};
	VECTOR2D			m_Direction{};

	const int			m_AnimationFrame{ 16 };
	float				m_AnimationTime{};

public:
	CBullet() = default;
	~CBullet() = default;

	void Animate();
	void Render(HDC hMemDC, HDC hMemDC2);

	bool IsCollided() const;
	void PrepareCollision();

	void SetAttackPower(float AttackPower);
	float GetAttackPower() const;

	void SetLength(float Length);
	float GetLength() const;

	void SetDirection(const VECTOR2D& Direction);
	void SetDirection(float DirX, float DirY);
	VECTOR2D GetDirection() const;
};

// ============================================== CPlayer ============================================== 

class CPlayer : public CGameObject
{
private:
	int					m_ID{};

	SOCKET			    m_Socket{};
	SOCKADDR_IN		    m_SocketAddress{};

	POINT				m_CameraStartPosition{};

	VECTOR2D			m_Direction{};

	const int			m_AnimationFrame{ 5 };
	float				m_AnimationTime{};

	bool				m_IsGetItem{};
	float				m_ItemDuration{};

	CBullet			    m_Bullets[MAX_BULLET]{};
	int					m_BulletIndex{};

public:
	CPlayer();
	~CPlayer() = default;

	void Animate();
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetID(int ID);
	int GetID() const;

	void SetSocket(SOCKET Socket);
	SOCKET GetSocket() const;

	void SetSocketAddress(const SOCKADDR_IN& SocketAddress);
	const SOCKADDR_IN& GetSocketAddress() const;

	const POINT& GetCameraStartPosition() const;

	void SetDirection(const VECTOR2D& Direction);
	void SetDirection(float DirX, float DirY);
	VECTOR2D GetDirection() const;

	bool IsGetItem() const;
	void ReinforceBullet();

	CBullet* GetBullets();
	void FireBullet(const POINT& CursorPos);

	void UpdateCamera(const RECT& ClientRect, const RECT& MapRect);
};
