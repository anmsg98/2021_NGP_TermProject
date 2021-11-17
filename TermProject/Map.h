#pragma once

class CMap
{
private:
	RECT			m_Rect{};
	USER_RECT		m_BitmapRect{};

public:
	CMap() = default;
	~CMap() = default;

	void Render(HDC hMemDC, HDC hMemDC2);

	void SetRect(const RECT& Rect);
	const RECT& GetRect() const;

	void SetBitmapRect(const USER_RECT& Rect);
	const USER_RECT& GetBitmapRect() const;
};
