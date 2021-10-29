#include "stdafx.h"
#include "Player.h"

const POSITION& CPlayer::GetCameraStartPosition() const
{
	return m_CameraStartPosition;
}

void CPlayer::Update(const RECT& ClientRect, const RECT& MapRect)
{
	// ���� �������� ���� ���̻� �������� �ʴ´�.
	m_CameraStartPosition.m_X = m_Position.m_X - (ClientRect.right / 2);
	m_CameraStartPosition.m_Y = m_Position.m_Y - (ClientRect.bottom / 2);

	if (m_CameraStartPosition.m_X < 0)
	{
		m_CameraStartPosition.m_X = 0;
	}
	else if (m_CameraStartPosition.m_X > MapRect.right - ClientRect.right)
	{
		m_CameraStartPosition.m_X = MapRect.right - ClientRect.right;
	}

	if (m_CameraStartPosition.m_Y < 0)
	{
		m_CameraStartPosition.m_Y = 0;
	}
	else if (m_CameraStartPosition.m_Y > MapRect.bottom - ClientRect.bottom)
	{
		m_CameraStartPosition.m_Y = MapRect.bottom - ClientRect.bottom;
	}
}
