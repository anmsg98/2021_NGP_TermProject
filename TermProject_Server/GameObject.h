#pragma once
#include "FileManager.h"

class CGameObject
{
protected:
	bool				m_IsActive{};

	int					m_Hp{};

	POSITION			m_Position{};
	float				m_Width{};
	float				m_Height{};

public:
	CGameObject() = default;
	~CGameObject() = default;

	void Animate(float DeltaTime);
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetHp(int Hp);
	int GetHp() const;

	void SetPosition(const POSITION& Position);
	void SetPosition(float Xpos, float Ypos);
	const POSITION& GetPosition() const;

	void SetWidth(float Width);
	float GetWidth() const;

	void SetHeight(float Height);
	float GetHeight() const;
};
