#pragma once
#include "GameObject.h"

#define MAX_MONSTER 30

class CMonster : public CGameObject
{
public:
	enum { LOWER = 1, MIDDLE, UPPER };

private:
	int				m_Type{};

	const int		m_AnimationFrame{ 5 };
	float			m_AnimationTime{};

	float			m_Length{};

	VECTOR2D		m_Direction{};
	VECTOR2D		m_PrevDirection{};

	bool			m_IsCollided{};
	float			m_CollisionDuration{};

public:
	CMonster() = default;
	~CMonster() = default;

	void Animate(float DeltaTime);
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetType(int Type);
	int GetType() const;

	void SetLength(float Length);
	float GetLength() const;

	void SetDirection(const VECTOR2D& Direction);
	void SetDirection(float DirX, float DirY);
	VECTOR2D GetDirection() const;

	void SetPrevDirection(const VECTOR2D& PrevDirection);
	void SetPrevDirection(float PrevDirX, float PrevDirY);
	VECTOR2D GetPrevDirection() const;

	bool IsCollided() const;
	void PrepareCollision();
};
