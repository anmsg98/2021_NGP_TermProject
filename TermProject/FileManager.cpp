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
	m_Bitmaps.insert(make_pair("BACKGROUND_W", LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_BG_W))));
	m_Bitmaps.insert(make_pair("BACKGROUND_I", LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_BG_I))));
	m_Bitmaps.insert(make_pair("SPRITE_SHEET", LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_SPRITE))));
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
