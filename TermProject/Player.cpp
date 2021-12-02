#include "stdafx.h"
#include "Player.h"

// ============================================== CBullet ==============================================

void CBullet::Animate()
{

}

void CBullet::Render(HDC hMemDC, HDC hMemDC2)
{
	if (m_IsActive)
	{
		USER_RECT BitmapRect{};

		if (m_IsCollided)
		{
			if (m_Sounded)
			{
				CSoundManager::GetInstance()->Play(CSoundManager::EXPLOSION_SOUND, 0.5f);
				m_Sounded = false;
			}

			int FrameIndex{ (int)m_AnimationTime % m_AnimationFrame };

			BitmapRect = CFileManager::GetInstance()->GetRect("EXPLOSION");
			BitmapRect.m_Left = BitmapRect.m_Left + BitmapRect.m_Width * (FrameIndex % 4);
			BitmapRect.m_Top = BitmapRect.m_Top + BitmapRect.m_Height * (FrameIndex / 4);

			DrawRect(hMemDC, GetPosition(), (float)BitmapRect.m_Width, (float)BitmapRect.m_Height, hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());
		}
		else
		{
			BitmapRect = CFileManager::GetInstance()->GetRect("BULLET");
			
			if (m_AttackPower > 10.0f)
			{
				BitmapRect.m_Left += BitmapRect.m_Width;
			}

			DrawRect(hMemDC, GetPosition(), GetWidth(), GetHeight(), hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());
		}
	}
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

}

void CPlayer::Animate()
{

}

void CPlayer::Render(HDC hMemDC, HDC hMemDC2)
{
	if (m_IsActive)
	{
		USER_RECT BitmapRect{};

		switch (m_ID)
		{
		case 0:
			BitmapRect = CFileManager::GetInstance()->GetRect("PLAYER_1");
			break;
		case 1:
			BitmapRect = CFileManager::GetInstance()->GetRect("PLAYER_2");
			break;
		case 2:
			BitmapRect = CFileManager::GetInstance()->GetRect("PLAYER_3");
			break;
		case 3:
			BitmapRect = CFileManager::GetInstance()->GetRect("PLAYER_4");
			break;
		}

		if (m_Hp <= 0.0f)
		{
			int FrameIndex{ (int)m_AnimationTime % m_AnimationFrame };
			BitmapRect.m_Left = BitmapRect.m_Width * FrameIndex;
		}

		HBITMAP hSourceBitmap{ CFileManager::GetInstance()->GetBitmap("SPRITE_SHEET") };
		HBITMAP hRotateBitmap{ GetRotatedBitmap(hMemDC, hSourceBitmap, BitmapRect.m_Left, BitmapRect.m_Top, BitmapRect.m_Width, BitmapRect.m_Height, atan2f(m_Direction.m_Y, m_Direction.m_X) * 180.0f / PI - 90.0f, CFileManager::GetInstance()->GetTransparentColor()) };
		USER_RECT Rect{ 0, 0, BitmapRect.m_Width, BitmapRect.m_Height };

		SelectObject(hMemDC2, hRotateBitmap);
		DrawRect(hMemDC, GetPosition(), 2 * GetWidth(), 2 * GetHeight(), hMemDC2, Rect, CFileManager::GetInstance()->GetTransparentColor());
		SelectObject(hMemDC2, hSourceBitmap);
		DeleteObject(hRotateBitmap);

		if (m_Hp > 0.0f)
		{
			// ü�¹�
			VECTOR2D Position{ GetPosition() };

			Position.m_Y -= 0.4f * BitmapRect.m_Height;
			BitmapRect = CFileManager::GetInstance()->GetRect("HP_1");
			DrawRect(hMemDC, Position, (float)BitmapRect.m_Width, (float)BitmapRect.m_Height, hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());

			BitmapRect = CFileManager::GetInstance()->GetRect("HP_2");
			float CurrentWidth{ BitmapRect.m_Width * (m_Hp / m_MaxHp) };
			FixedDrawRect(hMemDC, Position, (float)BitmapRect.m_Width, (float)BitmapRect.m_Height, CurrentWidth, (float)BitmapRect.m_Height, hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());
		}

		if (m_IsReady)
		{
			VECTOR2D Position{ GetPosition() };

			BitmapRect = CFileManager::GetInstance()->GetRect("BUTTON_READY");
			Position.m_Y += BitmapRect.m_Height;
			DrawRect(hMemDC, Position, (float)BitmapRect.m_Width, (float)BitmapRect.m_Height, hMemDC2, BitmapRect, CFileManager::GetInstance()->GetTransparentColor());
		}

#ifdef DEBUG_PRINT_HP
		TCHAR HpText[32]{};

		sprintf(HpText, "%.f", m_Hp);
		TextOut(hMemDC, (int)(m_Position.m_X - 15.0f), (int)(m_Position.m_Y - 0.5f * m_Size.m_Y), HpText, lstrlen(HpText));
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
		if (!m_Bullets[m_BulletIndex].IsCollided())
		{
			m_Bullets[m_BulletIndex].SetActive(true);
			m_Bullets[m_BulletIndex].SetDirection(CursorPos.x + GetCameraStartPosition().x - GetPosition().m_X, CursorPos.y + GetCameraStartPosition().y - GetPosition().m_Y);
			m_Bullets[m_BulletIndex].SetLength(sqrtf(powf(m_Bullets[m_BulletIndex].GetDirection().m_X, 2) + powf(m_Bullets[m_BulletIndex].GetDirection().m_Y, 2)));
			m_Bullets[m_BulletIndex].SetPosition(GetPosition().m_X + (m_Bullets[m_BulletIndex].GetDirection().m_X / m_Bullets[m_BulletIndex].GetLength()) * 0.5f * GetWidth(),
											     GetPosition().m_Y + (m_Bullets[m_BulletIndex].GetDirection().m_Y / m_Bullets[m_BulletIndex].GetLength()) * 0.5f * GetHeight());
	
			CSoundManager::GetInstance()->Play(CSoundManager::BULLET_SOUND, 0.5f);
		}
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
