#pragma once
#include "GameObject.h"

class CMonster : public CGameObject
{
public:
	enum { LOWER = 1, MIDDLE, UPPER };
	
private:
	int				m_Type{};

	float			m_Length{};
	VECTOR2D		m_Direction{};

	const int		m_AnimationFrame{ 5 };
	float			m_AnimationTime{};

	bool			m_IsCollided{};
	float			m_CollisionDuration{};

public:
	CMonster() = default;
	~CMonster() = default;

	void Animate();
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetType(int Type);
	int GetType() const;

	void SetLength(float Length);
	float GetLength() const;

	void SetDirection(const VECTOR2D& Direction);
	void SetDirection(float DirX, float DirY);
	const VECTOR2D& GetDirection() const;

	bool IsCollided() const;
	void SetCollision(bool IsCollided);
};
