#pragma once

class CFileManager
{
private:
	COLORREF								m_TransparentColor{ RGB(255, 174, 201) };

	unordered_map<string, USER_RECT>		m_Rects{};
	unordered_map<string, HBITMAP>			m_Bitmaps{};

public:
	CFileManager() = default;
	virtual ~CFileManager() = default;

	static CFileManager* GetInstance();

	COLORREF GetTransparentColor() const;

	void LoadBitmaps(HINSTANCE hInstance);
	void LoadRectFromFile(const char* FileName);

	USER_RECT GetRect(const string& Name);
	HBITMAP GetBitmap(const string& Name);
};
