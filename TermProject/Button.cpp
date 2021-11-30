#include "stdafx.h"
#include "Button.h"
#include "FileManager.h"

void CButton::Render(HDC hMemDC, HDC hMemDC2)
{
	USER_RECT BitmapRect{};
	
	switch (m_Type)
	{
	case READY:
		if (m_IsActive)
		{
			BitmapRect = CFileManager::GetInstance()->GetRect("BUTTON_READY_ON");
		}
		else
		{
			BitmapRect = CFileManager::GetInstance()->GetRect("BUTTON_READY_OFF");
		}
		break;
	case EXIT:
		if (m_IsActive)
		{
			BitmapRect = CFileManager::GetInstance()->GetRect("BUTTON_EXIT_ON");
		}
		else
		{
			BitmapRect = CFileManager::GetInstance()->GetRect("BUTTON_EXIT_OFF");
		}
		break;
	}

	DrawRect(hMemDC, GetPosition(), (float)BitmapRect.m_Width, (float)BitmapRect.m_Height, hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());
}

void CButton::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

bool CButton::IsActive() const
{
	return m_IsActive;
}

void CButton::SetType(int Type)
{
	m_Type = Type;
}

int CButton::GetType() const
{
	return m_Type;
}

void CButton::SetPosition(const VECTOR2D& Position)
{
	m_Position = Position;
}

void CButton::SetPosition(float Xpos, float Ypos)
{
	SetPosition(VECTOR2D(Xpos, Ypos));
}

const VECTOR2D& CButton::GetPosition() const
{
	return m_Position;
}

void CButton::SetSize(const VECTOR2D& Size)
{
	m_Size = Size;
}

const VECTOR2D& CButton::GetSize() const
{
	return m_Size;
}

void CButton::SetWidth(float Width)
{
	m_Size.m_X = Width;
}

float CButton::GetWidth() const
{
	return m_Size.m_X;
}

void CButton::SetHeight(float Height)
{
	m_Size.m_Y = Height;
}

float CButton::GetHeight() const
{
	return m_Size.m_Y;
}