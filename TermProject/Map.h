#pragma once

class CMap
{
private:
	RECT			m_Rect{};

public:
	CMap() = default;
	virtual ~CMap() = default;

	void SetRect(const RECT& Rect);
	const RECT& GetRect() const;
};
