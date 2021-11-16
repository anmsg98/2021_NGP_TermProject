#include "stdafx.h"
#include "Player.h"

// ============================================== CBullet ==============================================

void CBullet::Animate(float DeltaTime)
{
	if (m_IsActive)
	{
		m_Position.m_X += (GetDirection().x / GetLength()) * DeltaTime * 1000.f;
		m_Position.m_Y += (GetDirection().y / GetLength()) * DeltaTime * 1000.f;

		if (m_Position.m_X <= 0.0f || m_Position.m_X >= 2400.0f || m_Position.m_Y <= 0.0f || m_Position.m_Y >= 1500.0f)
		{
			m_IsActive = true;
		}
	}
}

void CBullet::Render(HDC hMemDC, HDC hMemDC2)
{
	if (m_IsActive)
	{
		USER_RECT Rect{};

		// 플레이어가 아이템을 획득하여 총알이 강화된 경우와 분리한다.
		if (m_AttackPower == 10.0f)
		{
			Rect = CFileManager::GetInstance()->GetRect("Bullet_1");
		}
		else if (m_AttackPower == 30.0f)
		{
			Rect = CFileManager::GetInstance()->GetRect("Bullet_2");
		}

		DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, Rect, CFileManager::GetInstance()->GetTransparentColor());
	}
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

void CBullet::SetDirection(float DirX, float DirY)
{
	m_Direction.x = (int)DirX;
	m_Direction.y = (int)DirY;
}

POINT CBullet::GetDirection() const
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
	if (m_IsActive)
	{
		USER_RECT Rect{ CFileManager::GetInstance()->GetRect("Player_1") };
		HBITMAP hSourceBitmap{ CFileManager::GetInstance()->GetBitmap("SpriteSheet") };
		HBITMAP hRotateBitmap{ GetRotatedBitmap(hMemDC, hSourceBitmap, 0, 0, Rect.m_Width, Rect.m_Height, atan2f((float)m_Direction.y, (float)m_Direction.x) * 180.0f / PI - 90.0f, CFileManager::GetInstance()->GetTransparentColor()) };

		SelectObject(hMemDC2, hRotateBitmap);
		DrawRect(hMemDC, GetPosition(), 2 * GetWidth(), 2 * GetHeight(), hMemDC2, Rect, CFileManager::GetInstance()->GetTransparentColor());
		SelectObject(hMemDC2, hSourceBitmap);

#ifdef DEBUG_HP
		TCHAR HpText[32]{};

		sprintf(HpText, "%.f", m_Hp);
		TextOut(hMemDC, (int)(m_Position.m_X - 15.0f), (int)(m_Position.m_Y - 0.5f * m_Height), HpText, lstrlen(HpText));
#endif

		for (int i = 0; i < MAX_BULLET; ++i)
		{
			m_Bullets[i].Render(hMemDC, hMemDC2);
		}
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

const POINT& CPlayer::GetCameraStartPosition() const
{
	return m_CameraStartPosition;
}

void CPlayer::SetDirection(float DirX, float DirY)
{
	m_Direction.x = (int)DirX;
	m_Direction.y = (int)DirY;
}

POINT CPlayer::GetDirection() const
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

void CPlayer::FireBullet(const POINT& CursorPos)
{
	if (m_Bullets)
	{
		m_Bullets[m_BulletIndex].SetActive(true);
		m_Bullets[m_BulletIndex].SetDirection(CursorPos.x + GetCameraStartPosition().x - GetPosition().m_X, CursorPos.y + GetCameraStartPosition().y - GetPosition().m_Y);
		m_Bullets[m_BulletIndex].SetLength(sqrtf(powf((float)m_Bullets[m_BulletIndex].GetDirection().x, 2) + powf((float)m_Bullets[m_BulletIndex].GetDirection().y, 2)));
		m_Bullets[m_BulletIndex].SetPosition(GetPosition().m_X + (m_Bullets[m_BulletIndex].GetDirection().x / m_Bullets[m_BulletIndex].GetLength()) * 0.5f * GetWidth(),
			GetPosition().m_Y + (m_Bullets[m_BulletIndex].GetDirection().y / m_Bullets[m_BulletIndex].GetLength()) * 0.5f * GetHeight());
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