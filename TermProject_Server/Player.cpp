#include "stdafx.h"
#include "Player.h"

// ============================================== CBullet ==============================================

void CBullet::Animate()
{
	if (m_IsActive)
	{
		if (m_IsCollided)
		{
			m_AnimationTime += 0.35f;

			if (m_AnimationTime >= m_AnimationFrame)
			{
				m_IsActive = false;
				m_IsCollided = false;
				m_AnimationTime = 0.0f;
			}

			if (m_Sounded)
			{
				m_Sounded = false;
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
	m_Sounded = true;
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
		m_Bullets[i].SetWidth(15.0f);
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
				if (m_ItemDuration >= 480.0f)
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

void CPlayer::ReinforceBullet(float Increment)
{
	m_GetItem = true;
	m_ItemDuration = 0.0f;

	for (int i = 0; i < MAX_BULLET; ++i)
	{
		m_Bullets[i].SetAttackPower(m_Bullets[i].GetAttackPower() + Increment);
	}
}

CBullet* CPlayer::GetBullets()
{
	return m_Bullets;
}

void CPlayer::FireBullet(const POINT& CursorPos)
{

}

void CPlayer::UpdateCamera(const RECT& ClientRect, const RECT& MapRect)
{

}

void CPlayer::IncreaseKillCount()
{
	++m_KillCount;
}

void CPlayer::ResetKillCount()
{
	m_KillCount = 0;
}

int CPlayer::GetKillCount() const
{
	return m_KillCount;
}

void CPlayer::IncreaseDamageDealt(int Damage)
{
	m_DamageDealt += Damage;
}

void CPlayer::ResetDamageDealt()
{
	m_DamageDealt = 0;
}

int CPlayer::GetDamageDealt() const
{
	return m_DamageDealt;
}

int CPlayer::CalculateRank() const
{
	int Rank{ 100 * m_KillCount + m_DamageDealt };

	// ���� ���� ��Ʈ�� ��ġ�� �������� ���̴�.
	if (Rank < 1000)
	{
		return 4;
	}
	else if (Rank < 3000)
	{
		return 3;
	}
	else if (Rank < 7000)
	{
		return 2;
	}
	else if (Rank < 10000)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
