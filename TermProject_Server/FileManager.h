#pragma once

class CFileManager
{
private:
	const COLORREF							m_TransparentColor{ RGB(1, 1, 1) };

	unordered_map<string, USER_RECT>		m_Rects{};
	unordered_map<string, HBITMAP>			m_Bitmaps{};

public:
	CFileManager() = default;
	~CFileManager() = default;

	static CFileManager* GetInstance();

	COLORREF GetTransparentColor() const;

	void LoadBitmaps(HINSTANCE hInstance);
	void LoadRectFromFile(const char* FileName);

	USER_RECT GetRect(const string& Name);
	HBITMAP GetBitmap(const string& Name);
};
