#include "stdafx.h"
#include "Player.h"

// ============================================== CBullet ==============================================

void CBullet::Update(float DeltaTime)
{
	if (m_IsActive)
	{
		m_Position.m_X += 300.0f * DeltaTime;

		if (m_Position.m_X >= 2000)
		{
			SetActive(false);
		}
	}
}

// ============================================== CPlayer ==============================================

CPlayer::CPlayer()
{
	m_Bullets = new CBullet[MAX_BULLET]{};

	for (int i = 0; i < MAX_BULLET; ++i)
	{
		m_Bullets[i].SetWidth(20.0f);
		m_Bullets[i].SetHeight(15.0f);
	}
}

CPlayer::~CPlayer()
{
	if (m_Bullets)
	{
		delete[] m_Bullets;
	}
}

const POINT& CPlayer::GetCameraStartPosition() const
{
	return m_CameraStartPosition;
}

CBullet* CPlayer::GetBullets()
{
	return m_Bullets;
}

void CPlayer::FireBullet()
{
	if (m_Bullets)
	{
		m_Bullets[m_BulletIndex].SetActive(true);
		m_Bullets[m_BulletIndex].SetPosition(CPlayer::GetPosition().m_X, CPlayer::GetPosition().m_Y);

		m_BulletIndex = (m_BulletIndex + 1) % MAX_BULLET;
	}
}

void CPlayer::Animate(float DeltaTime)
{
	for (int i = 0; i < MAX_BULLET; ++i)
	{
		m_Bullets[i].Update(DeltaTime);
	}
}

void CPlayer::UpdateCamera(const RECT& ClientRect, const RECT& MapRect)
{
	// 맵의 구석으로 가면 더이상 움직이지 않는다.
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
