#pragma once
#include "FileManager.h"

class CGameObject
{
protected:
	bool				m_IsActive{};

	float				m_Hp{};

	POSITION			m_Position{};

	USER_RECT			m_BitmapRect{};
	float				m_Width{};
	float				m_Height{};

public:
	CGameObject() = default;
	~CGameObject() = default;

	void Animate(float DeltaTime);
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetHp(float Hp);
	float GetHp() const;

	void SetPosition(const POSITION& Position);
	void SetPosition(float Xpos, float Ypos);
	const POSITION& GetPosition() const;

	void SetBitmapRect(const USER_RECT& Rect);
	const USER_RECT& GetBitmapRect() const;

	void SetWidth(float Width);
	float GetWidth() const;

	void SetHeight(float Height);
	float GetHeight() const;
};
