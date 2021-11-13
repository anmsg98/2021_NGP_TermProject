#include "stdafx.h"
#include "Player.h"

// ============================================== CBullet ==============================================

void CBullet::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		m_Position.m_X += (GetDirect().x / GetLength()) * DeltaTime * 1000.f;
		m_Position.m_Y += (GetDirect().y / GetLength()) * DeltaTime * 1000.f;

		if (m_Position.m_X <= 0.0f || m_Position.m_X >= 2400.0f || m_Position.m_Y <= 0.0f || m_Position.m_Y >= 1500.0f)
		{
			m_IsActive = true;
		}
	}
}

void CBullet::Render(HDC hMemDC, HDC hMemDC2)
{

}

void CBullet::SetAttackPower(int AttackPower)
{
	m_AttackPower = AttackPower;
}

int CBullet::GetAttackPower() const
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

void CBullet::SetDirect(float DirX, float DirY)
{
	m_Direction.x = (int)DirX;
	m_Direction.y = (int)DirY;
}

POINT CBullet::GetDirect() const
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

void CPlayer::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		printf("%.02f, %.02f\r", GetPosition().m_X, GetPosition().m_Y);

		if (m_IsReinforced)
		{
			m_ItemDuration += DeltaTime;

			// �������� ���ӽð��� �Ѿ�� ������� �ǵ�����.
			if (m_ItemDuration >= 8.0f)
			{
				m_IsReinforced = false;
				m_ItemDuration = 0.0f;

				for (int i = 0; i < MAX_BULLET; ++i)
				{
					m_Bullets[i].SetAttackPower(10);
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

const POINT& CPlayer::GetCameraStartPosition() const
{
	return m_CameraStartPosition;
}

CBullet* CPlayer::GetBullets()
{
	return m_Bullets;
}

void CPlayer::SetDirect(float DirX, float DirY)
{
	m_Direction.x = (int)DirX;
	m_Direction.y = (int)DirY;
}

POINT CPlayer::GetDirect() const
{
	return m_Direction;
}

void CPlayer::ReinforceBullet()
{
	m_IsReinforced = true;
	m_ItemDuration = 0.0f;

	for (int i = 0; i < MAX_BULLET; ++i)
	{
		m_Bullets[i].SetAttackPower(30);
	}
}

bool CPlayer::IsReinforced() const
{
	return m_IsReinforced;
}

void CPlayer::FireBullet(const POINT& CursorPos)
{
	if (m_Bullets)
	{
		m_Bullets[m_BulletIndex].SetActive(true);
		m_Bullets[m_BulletIndex].SetDirect(CursorPos.x + GetCameraStartPosition().x - GetPosition().m_X, CursorPos.y + GetCameraStartPosition().y - GetPosition().m_Y);
		m_Bullets[m_BulletIndex].SetLength(sqrtf(powf((float)m_Bullets[m_BulletIndex].GetDirect().x, 2) + powf((float)m_Bullets[m_BulletIndex].GetDirect().y, 2)));
		m_Bullets[m_BulletIndex].SetPosition(GetPosition().m_X + (m_Bullets[m_BulletIndex].GetDirect().x / m_Bullets[m_BulletIndex].GetLength()) * 0.5f * GetWidth(),
											 GetPosition().m_Y + (m_Bullets[m_BulletIndex].GetDirect().y / m_Bullets[m_BulletIndex].GetLength()) * 0.5f * GetHeight());
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