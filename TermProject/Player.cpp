#include "stdafx.h"
#include "Player.h"

const POINT& CPlayer::GetCameraStartPosition() const
{
	return m_CameraStartPosition;
}

void CPlayer::Update(const RECT& ClientRect, const RECT& MapRect)
{
	// ���� �������� ���� ���̻� �������� �ʴ´�.
	m_CameraStartPosition.x = m_Position.m_X - (ClientRect.right / 2);
	m_CameraStartPosition.y = m_Position.m_Y - (ClientRect.bottom / 2);

	if (m_CameraStartPosition.x < 0)
	{
		m_CameraStartPosition.x = 0;
	}
	else if (m_CameraStartPosition.x > MapRect.right - ClientRect.right)
	{
		m_CameraStartPosition.x = MapRect.right - ClientRect.right;
	}

	if (m_CameraStartPosition.y < 0)
	{
		m_CameraStartPosition.y = 0;
	}
	else if (m_CameraStartPosition.y > MapRect.bottom - ClientRect.bottom)
	{
		m_CameraStartPosition.y = MapRect.bottom - ClientRect.bottom;
	}
}
