#pragma once

class CButton
{
public:
	enum { READY = 1, EXIT };

private:
	bool				m_IsActive{};

	int					m_Type;

	VECTOR2D			m_Position{};
	VECTOR2D			m_Size{};

public:
	CButton() = default;
	~CButton() = default;

	void Render(HDC hMemDC, HDC hMemDC2);

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetType(int Type);
	int GetType() const;

	void SetPosition(const VECTOR2D& Position);
	void SetPosition(float Xpos, float Ypos);
	const VECTOR2D& GetPosition() const;

	void SetSize(const VECTOR2D& Size);
	const VECTOR2D& GetSize() const;

	void SetWidth(float Width);
	float GetWidth() const;

	void SetHeight(float Height);
	float GetHeight() const;
};
