#include "stdafx.h"
#include "Scene.h"
#include "Map.h"
#include "Player.h"
#include "Monster.h"
#include "FileManager.h"

CGameScene::~CGameScene()
{
	if (m_Map)
	{
		delete m_Map;
	}

	if (m_Player)
	{
		delete m_Player;
	}

	for (int i = 0; i < m_Monsters.size(); ++i) {
		if (m_Monsters[i])
		{
			delete m_Monsters[i];
		}
	}
}

void CGameScene::ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		break;
	default:
		break;
	}
}

void CGameScene::ProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		m_CursorPos.x = LOWORD(lParam);
		m_CursorPos.y = HIWORD(lParam);

		if (m_Player)
		{
			m_Player->FireBullet(m_CursorPos);
		}
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	}
}

void CGameScene::ProcessInput(float DeltaTime)
{
	float Speed{ 300.0f * DeltaTime };
	RECT MapRect{ m_Map->GetRect() };
	POSITION PlayerPos{ m_Player->GetPosition() };

	if (GetAsyncKeyState('W') & 0x8000)
	{
		if (PlayerPos.m_Y > MapRect.top + 0.5f * m_Player->GetHeight())
		{
			PlayerPos.m_Y -= Speed;
		}
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		if (PlayerPos.m_Y < MapRect.bottom - 0.5f * m_Player->GetHeight())
		{
			PlayerPos.m_Y += Speed;
		}
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		if (PlayerPos.m_X > MapRect.left + 0.5f * m_Player->GetWidth())
		{
			PlayerPos.m_X -= Speed;
		}
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		if (PlayerPos.m_X < MapRect.right - 0.5f * m_Player->GetWidth())
		{
			PlayerPos.m_X += Speed;
		}
	}

	m_Player->SetPosition(PlayerPos);
}

void CGameScene::OnCreate(HINSTANCE hInstance, HWND hWnd)
{
	CFileManager::GetInstance()->LoadBitmaps(hInstance);
	CFileManager::GetInstance()->LoadRectFromFile("Image/SpriteCoord.txt");

	GetClientRect(hWnd, &m_ClientRect);
	BuildObject();
}

void CGameScene::OnDestroy()
{

}

void CGameScene::BuildObject()
{
	RECT MapRect{ 0, 0, 2000, 1000 };

	// 맵을 생성한다.
	m_Map = new CMap();
	m_Map->SetRect(MapRect);

	// 플레이어를 생성한다.
	m_Player = new CPlayer();
	m_Player->SetPosition(50.0f, 50.0f);
	m_Player->SetWidth(62.5f);
	m_Player->SetHeight(90.0f);
}

void CGameScene::Animate(float DeltaTime)
{
	if (m_Player)
	{
		m_Player->Animate(DeltaTime);
		m_Player->UpdateCamera(m_ClientRect, m_Map->GetRect());
	}

	for (int i = 0; i < m_Monsters.size(); i++)
	{
		if (m_Monsters[i])
		{
			if (m_Monsters[i]->IsActive())
			{
				m_Monsters[i]->Animate(DeltaTime);
			}
		}
	}

	CreateMonster(DeltaTime);
	CheckMonsterByBulletCollision();
}

void CGameScene::CreateMonster(float DeltaTime)
{
	m_CurrentGenTime += DeltaTime;

	if (m_CurrentGenTime >= m_MonsterGenTime)
	{
		CMonster* Monster = new CMonster();

		Monster->SetActive(true);
		Monster->SetWidth(73.5f);
		Monster->SetHeight(76.0f);

		// 타입결정
		int Random{ rand() % 3 + 1 };

		Monster->SetType(Random);

		// 생성지점 결정
		Random = rand() % 4;

		switch (Random)
		{
		case 0:
			Monster->SetPosition((float)m_Map->GetRect().left, RandF((float)m_Map->GetRect().top, (float)m_Map->GetRect().bottom));
			break;
		case 1:
			Monster->SetPosition((float)m_Map->GetRect().right, RandF((float)m_Map->GetRect().top, (float)m_Map->GetRect().bottom));
			break;
		case 2:
			Monster->SetPosition(RandF((float)m_Map->GetRect().left, (float)m_Map->GetRect().right), (float)m_Map->GetRect().top);
			break;
		case 3:
			Monster->SetPosition(RandF((float)m_Map->GetRect().left, (float)m_Map->GetRect().right), (float)m_Map->GetRect().bottom);
			break;
		}
		
		// 맵의 중앙을 도착점으로 지정
		RECT MapRect{ m_Map->GetRect() };

		Monster->SetDirect((float)MapRect.right * 0.5f - Monster->GetPosition().m_X, (float)MapRect.bottom * 0.5f - Monster->GetPosition().m_Y);
		Monster->SetLength(sqrtf(powf((float)Monster->GetDirect().x, 2) + powf((float)Monster->GetDirect().y, 2)));

		m_Monsters.push_back(Monster);
		m_CurrentGenTime = 0.0f;
	}
}

void CGameScene::CheckMonsterByBulletCollision()
{
	CBullet* Bullets{ m_Player->GetBullets() };

	for (int i = 0; i < MAX_BULLET; ++i)
	{
		for (int j = 0; j < m_Monsters.size(); ++j)
		{
			if (Bullets[i].IsActive() && m_Monsters[j]->IsActive())
			{
				RECT CollidedRect{};
				RECT BulletRect{ (int)(Bullets[i].GetPosition().m_X - 0.5f * Bullets[i].GetWidth()), (int)(Bullets[i].GetPosition().m_Y - 0.5f * Bullets[i].GetHeight()),
								 (int)(Bullets[i].GetPosition().m_X + 0.5f * Bullets[i].GetWidth()), (int)(Bullets[i].GetPosition().m_Y + 0.5f * Bullets[i].GetHeight()) };
				RECT MonsterRect{ (int)(m_Monsters[j]->GetPosition().m_X - 0.5f * m_Monsters[j]->GetWidth()), (int)(m_Monsters[j]->GetPosition().m_Y - 0.5f * m_Monsters[j]->GetHeight()),
								  (int)(m_Monsters[j]->GetPosition().m_X + 0.5f * m_Monsters[j]->GetWidth()), (int)(m_Monsters[j]->GetPosition().m_Y + 0.5f * m_Monsters[j]->GetHeight()) };

				if (IntersectRect(&CollidedRect, &BulletRect, &MonsterRect))
				{
					Bullets[i].SetActive(false);
					m_Monsters[j]->SetActive(false);
				}
			}
		}
	}
}

void CGameScene::Render(HDC hDC, HDC hMemDC, HDC hMemDC2)
{
	m_hBitmap = CreateCompatibleBitmap(hDC, m_Map->GetRect().right, m_Map->GetRect().bottom);
	m_hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

	SelectObject(hMemDC2, CFileManager::GetInstance()->GetBitmap("Background"));
	m_Map->Render(hMemDC, hMemDC2);

	SelectObject(hMemDC2, CFileManager::GetInstance()->GetBitmap("SpriteSheet"));
	m_Player->Render(hMemDC, hMemDC2);

	for (const auto& Monster : m_Monsters)
	{
		Monster->Render(hMemDC, hMemDC2);
	}

	POINT PlayerCameraPos{ m_Player->GetCameraStartPosition() };

	BitBlt(hDC, 0, 0, m_ClientRect.right, m_ClientRect.bottom, hMemDC, PlayerCameraPos.x, PlayerCameraPos.y, SRCCOPY);
	SelectObject(hMemDC, m_hBitmap);
	DeleteObject(m_hBitmap);
}
