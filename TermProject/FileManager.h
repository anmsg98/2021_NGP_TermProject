#pragma once

class CFileManager
{
private:
	COLORREF							m_TransColor{ RGB(255, 174, 201) };
	unordered_map<string, LTWH>			m_ImageRect{};

public:
	CFileManager() = default;
	virtual ~CFileManager() = default;

	const COLORREF& GetTransColor() const;

	void SetImageRectFromFile(const char* FileName);
	LTWH GetImageRect(const string& Name);
};
