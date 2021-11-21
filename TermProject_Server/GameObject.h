#pragma once
#include "FileManager.h"

class CGameObject
{
protected:
	bool				m_IsActive{};

	float				m_MaxHp{};
	float				m_Hp{};

	POSITION			m_Position{};
	VECTOR2D			m_Size{};

public:
	CGameObject() = default;
	~CGameObject() = default;

	void Animate(float DeltaTime);
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetMaxHp(float Hp);
	float GetMaxHp() const;

	void SetHp(float Hp);
	float GetHp() const;

	void SetPosition(const POSITION& Position);
	void SetPosition(float Xpos, float Ypos);
	const POSITION& GetPosition() const;

	void SetSize(const VECTOR2D& Size);
	const VECTOR2D& GetSize() const;

	void SetWidth(float Width);
	float GetWidth() const;

	void SetHeight(float Height);
	float GetHeight() const;
};
