#pragma once

class CMap
{
private:
	RECT			m_Rect{};

public:
	CMap() = default;
	~CMap() = default;

	void Render(HDC hMemDC, HDC hMemDC2);

	void SetRect(const RECT& Rect);
	const RECT& GetRect() const;
};
