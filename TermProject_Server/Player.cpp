#include "stdafx.h"
#include "Player.h"

// ============================================== CBullet ==============================================

void CBullet::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		m_Position.m_X += (GetDirection().m_X / GetLength()) * DeltaTime * 1000.f;
		m_Position.m_Y += (GetDirection().m_Y / GetLength()) * DeltaTime * 1000.f;

		if (m_Position.m_X <= 0.0f || m_Position.m_X >= 2400.0f || m_Position.m_Y <= 0.0f || m_Position.m_Y >= 1500.0f)
		{
			m_IsActive = false;
		}
	}
}

void CBullet::Render(HDC hMemDC, HDC hMemDC2)
{

}

void CBullet::SetAttackPower(float AttackPower)
{
	m_AttackPower = AttackPower;
}

float CBullet::GetAttackPower() const
{
	return m_AttackPower;
}

void CBullet::SetLength(float Length)
{
	m_Length = Length;
}

float CBullet::GetLength() const
{
	return m_Length;
}

void CBullet::SetDirection(const VECTOR2D& Direction)
{
	m_Direction = Direction;
}

void CBullet::SetDirection(float DirX, float DirY)
{
	SetDirection(VECTOR2D(DirX, DirY));
}

VECTOR2D CBullet::GetDirection() const
{
	return m_Direction;
}

// ============================================== CPlayer ==============================================

CPlayer::CPlayer()
{
	for (int i = 0; i < MAX_BULLET; ++i)
	{
		m_Bullets[i].SetWidth(20.0f);
		m_Bullets[i].SetHeight(15.0f);
		m_Bullets[i].SetBitmapRect(CFileManager::GetInstance()->GetRect("Bullet_1"));
	}
}

void CPlayer::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		if (m_IsGetItem)
		{
			m_ItemDuration += DeltaTime;
			
			// 아이템의 지속시간이 넘어가면 원래대로 되돌린다.
			if (m_ItemDuration >= 8.0f)
			{
				m_IsGetItem = false;
				m_ItemDuration = 0.0f;

				for (int i = 0; i < MAX_BULLET; ++i)
				{
					m_Bullets[i].SetAttackPower(10.0f);
				}
			}
		}

		for (int i = 0; i < MAX_BULLET; ++i)
		{
			m_Bullets[i].Animate(DeltaTime);
		}
	}
}

void CPlayer::Render(HDC hMemDC, HDC hMemDC2)
{

}

void CPlayer::SetID(int ID)
{
	m_ID = ID;
}

int CPlayer::GetID() const
{
	return m_ID;
}

void CPlayer::SetSocket(SOCKET Socket)
{
	m_Socket = Socket;
}

SOCKET CPlayer::GetSocket() const
{
	return m_Socket;
}

void CPlayer::SetSocketAddress(const SOCKADDR_IN& SocketAddress)
{
	m_SocketAddress = SocketAddress;
}

const SOCKADDR_IN& CPlayer::GetSocketAddress() const
{
	return m_SocketAddress;
}

const POINT& CPlayer::GetCameraStartPosition() const
{
	return m_CameraStartPosition;
}

void CPlayer::SetDirection(const VECTOR2D& Direction)
{
	m_Direction = Direction;
}

void CPlayer::SetDirection(float DirX, float DirY)
{
	SetDirection(VECTOR2D(DirX, DirY));
}

VECTOR2D CPlayer::GetDirection() const
{
	return m_Direction;
}

bool CPlayer::IsGetItem() const
{
	return m_IsGetItem;
}

void CPlayer::ReinforceBullet()
{
	m_IsGetItem = true;
	m_ItemDuration = 0.0f;

	for (int i = 0; i < MAX_BULLET; ++i)
	{
		m_Bullets[i].SetAttackPower(30.0f);
	}
}

CBullet* CPlayer::GetBullets()
{
	return m_Bullets;
}

void CPlayer::FireBullet(const POINT& CursorPos)
{
	if (m_Bullets)
	{
		m_Bullets[m_BulletIndex].SetActive(true);
		m_Bullets[m_BulletIndex].SetDirection(CursorPos.x + GetCameraStartPosition().x - GetPosition().m_X, CursorPos.y + GetCameraStartPosition().y - GetPosition().m_Y);
		m_Bullets[m_BulletIndex].SetLength(sqrtf(powf(m_Bullets[m_BulletIndex].GetDirection().m_X, 2) + powf(m_Bullets[m_BulletIndex].GetDirection().m_Y, 2)));
		m_Bullets[m_BulletIndex].SetPosition(GetPosition().m_X + (m_Bullets[m_BulletIndex].GetDirection().m_X / m_Bullets[m_BulletIndex].GetLength()) * 0.5f * GetWidth(),
											 GetPosition().m_Y + (m_Bullets[m_BulletIndex].GetDirection().m_Y / m_Bullets[m_BulletIndex].GetLength()) * 0.5f * GetHeight());
	}

	m_BulletIndex = (m_BulletIndex + 1) % MAX_BULLET;
}

void CPlayer::UpdateCamera(const RECT& ClientRect, const RECT& MapRect)
{
	if (m_IsActive)
	{
		// 맵의 구석으로 가면 더이상 움직이지 않는다.
		m_CameraStartPosition.x = (int)(m_Position.m_X - (0.5f * ClientRect.right));
		m_CameraStartPosition.y = (int)(m_Position.m_Y - (0.5f * ClientRect.bottom));

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
}
