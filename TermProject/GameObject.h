#pragma once

class CGameObject
{
protected:
	POSITION			m_Position{};
	float				m_Width{};
	float				m_Height{};

public:
	CGameObject();
	virtual ~CGameObject();

	void SetPosition(const POSITION& Position);
	void SetPosition(float Xpos, float Ypos);
	const POSITION& GetPosition() const;

	void SetWidth(float Width);
	float GetWidth() const;

	void SetHeight(float Height);
	float GetHeight() const;
};
