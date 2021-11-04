#pragma once

class CMap
{
private:
	RECT			m_Rect{};

public:
	CMap() = default;
	~CMap() = default;

	void SetRect(const RECT& Rect);
	const RECT& GetRect() const;

	void Render(HDC hMemDC, HDC hMemDC2);
};
