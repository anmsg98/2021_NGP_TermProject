#include "stdafx.h"
#include "Player.h"

// ============================================== CBullet ==============================================

void CBullet::Animate()
{
	if (m_IsActive)
	{
		if (m_IsCollided)
		{
			m_AnimationTime += 0.25f;

			if (m_AnimationTime >= m_AnimationFrame)
			{
				m_IsActive = false;
				m_IsCollided = false;
				m_AnimationTime = 0.0f;
			}
		}
		else
		{
			m_Position.m_X += (GetDirection().m_X / GetLength()) * 10.0f;
			m_Position.m_Y += (GetDirection().m_Y / GetLength()) * 10.0f;

			if (m_Position.m_X <= 0.0f || m_Position.m_X >= 2400.0f || m_Position.m_Y <= 0.0f || m_Position.m_Y >= 1500.0f)
			{
				m_IsActive = false;
			}
		}
	}
}

void CBullet::Render(HDC hMemDC, HDC hMemDC2)
{

}

bool CBullet::IsCollided() const
{
	return m_IsCollided;
}

void CBullet::PrepareCollision()
{
	m_IsCollided = true;
	m_AnimationTime = 0.0f;
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
	}
}

void CPlayer::Animate()
{
	if (m_IsActive)
	{
		if (m_Hp > 0.0f)
		{
			if (m_GetItem)
			{
				m_ItemDuration += 1.0f;

				// �������� ���ӽð��� �Ѿ�� ������� �ǵ�����.
				if (m_ItemDuration >= 10.0f)
				{
					m_GetItem = false;
					m_ItemDuration = 0.0f;

					for (int i = 0; i < MAX_BULLET; ++i)
					{
						m_Bullets[i].SetAttackPower(10.0f);
					}
				}
			}

			for (int i = 0; i < MAX_BULLET; ++i)
			{
				m_Bullets[i].Animate();
			}
		}
		else
		{
			m_AnimationTime += 0.25f;

			if (m_AnimationTime >= m_AnimationFrame)
			{
				m_IsActive = false;
				m_AnimationTime = 0.0f;
			}
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

void CPlayer::SetReady(bool IsReady)
{
	m_IsReady = IsReady;
}

bool CPlayer::IsReady() const
{
	return m_IsReady;
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
	return m_GetItem;
}

void CPlayer::ReinforceBullet()
{
	m_GetItem = true;
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
		// ���� �������� ���� ���̻� �������� �ʴ´�.
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
