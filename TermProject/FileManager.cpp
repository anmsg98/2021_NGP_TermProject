#include "stdafx.h"
#include "FileManager.h"

const COLORREF& CFileManager::GetTransColor() const
{
	return m_TransColor;
}

void CFileManager::SetImageRectFromFile(const char* FileName)
{
	ifstream InFile{ FileName };
	string Str{}, Name{};
	LTWH ltwh{};

	while (InFile >> Str)
	{
		if (Str == "<NAME>:")
		{
			InFile >> Name;
		}
		else if (Str == "<LTWH>:")
		{
			InFile >> ltwh.m_Left >> ltwh.m_Top >> ltwh.m_Width >> ltwh.m_Height;

			m_ImageRect.insert(make_pair(Name, ltwh));
		}
	}
}

LTWH CFileManager::GetImageRect(const string& Name)
{
	return m_ImageRect[Name];
}
