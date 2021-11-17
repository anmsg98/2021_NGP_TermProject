#include "stdafx.h"
#include "FileManager.h"

CFileManager* CFileManager::GetInstance()
{
	static CFileManager m_Instance{};

	return &m_Instance;
}

COLORREF CFileManager::GetTransparentColor() const
{
	return m_TransparentColor;
}

void CFileManager::LoadBitmaps(HINSTANCE hInstance)
{

}

void CFileManager::LoadRectFromFile(const char* FileName)
{
	ifstream InFile{ FileName };
	string Str{}, Name{};
	USER_RECT Rect{};

	while (InFile >> Str)
	{
		if (Str == "<NAME>:")
		{
			InFile >> Name;
		}
		else if (Str == "<LTWH>:")
		{
			InFile >> Rect.m_Left >> Rect.m_Top >> Rect.m_Width >> Rect.m_Height;

			m_Rects.insert(make_pair(Name, Rect));
		}
	}
}

USER_RECT CFileManager::GetRect(const string& Name)
{
	return m_Rects[Name];
}

HBITMAP CFileManager::GetBitmap(const string& Name)
{
	return m_Bitmaps[Name];
}
