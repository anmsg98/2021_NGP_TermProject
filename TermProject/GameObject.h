#pragma once
#include "FileManager.h"

class CGameObject
{
protected:
	bool				m_IsActive{};

	POSITION			m_Position{};
	float				m_Width{};
	float				m_Height{};

public:
	CGameObject() = default;
	virtual ~CGameObject() = default;

	virtual void Animate(float DeltaTime);
	virtual void Render(HDC hMemDC, HDC hMemDC2);

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetPosition(const POSITION& Position);
	void SetPosition(float Xpos, float Ypos);
	const POSITION& GetPosition() const;

	void SetWidth(float Width);
	float GetWidth() const;

	void SetHeight(float Height);
	float GetHeight() const;
};
