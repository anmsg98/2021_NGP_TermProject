#pragma once

class CGameObject
{
protected:
	bool				m_IsActive{};

	float				m_MaxHp{};
	float				m_Hp{};

	VECTOR2D			m_Position{};
	VECTOR2D			m_Size{};

	bool				m_Sounded{};

public:
	CGameObject() = default;
	~CGameObject() = default;

	void Animate();
	void Render(HDC hMemDC, HDC hMemDC2);

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetMaxHp(float Hp);
	float GetMaxHp() const;

	void SetHp(float Hp);
	float GetHp() const;

	void SetPosition(const VECTOR2D& Position);
	void SetPosition(float Xpos, float Ypos);
	const VECTOR2D& GetPosition() const;

	void SetSize(const VECTOR2D& Size);
	const VECTOR2D& GetSize() const;

	void SetWidth(float Width);
	float GetWidth() const;

	void SetHeight(float Height);
	float GetHeight() const;

	void SetSound(bool Sounded);
	bool GetSound() const;
};
