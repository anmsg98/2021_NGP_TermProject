#pragma once
#include "GameObject.h"

#define MAX_MONSTER 30

class CMonster : public CGameObject
{
public:
	enum { LOWER = 1, MIDDLE, UPPER };
	
private:
	float			m_TimeElapsed_T{};
	float			m_TimeElapsed_B{};

	bool			is_TowerCollide{};
	bool			is_BulletCollide{};

	int				m_Type{};

	const int		m_AnimationFrame{ 5 };
	float			m_AnimationTime{};

	float			m_Length{};

	POINT			m_Direction{};
	POINT			m_MapPosition{};

public:
	CMonster() = default;
	~CMonster() = default;

	void Animate(float DeltaTime);
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetType(int Type);
	int GetType() const;

	void SetLength(float Length);
	float GetLength() const;

	void SetMapPosition(float MapPosx, float MapPosy) { m_MapPosition.x = MapPosx, m_MapPosition.y = MapPosy; }
	POINT GetMapPosition() const;

	void SetBulletCollide() { is_BulletCollide = true; }
	void SetTowerCollide() { is_TowerCollide = true; }

	bool GetBulletCollide() { return is_BulletCollide; }
	bool GetTowerCollide() { return is_TowerCollide; }

	void SetDirection(float DirX, float DirY);
	POINT GetDirection() const;
};
