#pragma once
#include "GameObject.h"

#define MAX_PLAYER 4
#define MAX_BULLET 10

// ============================================== CBullet ==============================================

class CBullet : public CGameObject
{
private:
	int				m_AttackPower{ 10 };

	float			m_Length{};
	POINT			m_Direction{};

public:
	CBullet() = default;
	~CBullet() = default;

	virtual void Animate(float DeltaTime);
	virtual void Render(HDC hMemDC, HDC hMemDC2);

	void SetAttackPower(int AttackPower);
	int GetAttackPower() const;

	void SetLength(float Length);
	float GetLength() const;

	void SetDirect(float DirX, float DirY);
	POINT GetDirect() const;
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

	bool				m_IsReinforced{};
	float				m_ItemDuration{};

	CBullet			    m_Bullets[MAX_BULLET]{};
	int					m_BulletIndex{};

public:
	CPlayer();
	~CPlayer() = default;

	virtual void Animate(float DeltaTime);
	virtual void Render(HDC hMemDC, HDC hMemDC2);

	void SetID(int ID);
	int GetID() const;

	void SetSocket(SOCKET Socket);
	SOCKET GetSocket() const;

	void SetSocketAddress(const SOCKADDR_IN& SocketAddress);
	const SOCKADDR_IN& GetSocketAddress() const;

	const POINT& GetCameraStartPosition() const;
	CBullet* GetBullets();

	void SetDirect(float DirX, float DirY);
	POINT GetDirect() const;

	void ReinforceBullet();
	bool IsReinforced() const;

	void FireBullet(const POINT& CursorPos);

	void UpdateCamera(const RECT& ClientRect, const RECT& MapRect);
};