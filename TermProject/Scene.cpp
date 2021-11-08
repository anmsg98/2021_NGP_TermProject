#include "stdafx.h"
#include "Scene.h"
#include "Map.h"
#include "Tower.h"
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

	for (int i = 0; i < m_Monsters.size(); ++i)
	{
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
	// 맵을 생성한다.
	RECT MapRect{ 0, 0, 2400, 1500 };

	m_Map = new CMap{};
	m_Map->SetRect(MapRect);

	// 타워를 생성한다.
	m_Tower = new CTower{};
	m_Tower->SetActive(true);
	m_Tower->SetHp(5000);
	m_Tower->SetPosition(1200.0f, 740.0f);
	m_Tower->SetWidth(228.0f);
	m_Tower->SetHeight(213.0f);

	// 플레이어를 생성한다.
	m_Player = new CPlayer();
	m_Player->SetActive(true);
	m_Player->SetHp(100);
	m_Player->SetPosition(50.0f, 50.0f);
	m_Player->SetWidth(62.5f);
	m_Player->SetHeight(90.0f);

	// 몬스터를 생성한다.
	const int MonsterCount{ 10 };

	for (int i = 0; i < MonsterCount; ++i)
	{
		CMonster* Monster{ new CMonster{} };

		Monster->SetActive(true);
		Monster->SetWidth(73.5f);
		Monster->SetHeight(76.0f);

		// 몬스터의 종류 설정
		int Type{ rand() % 3 + 1 };

		Monster->SetType(Type);
		Monster->SetHp(10 * Type);

		// 몬스터의 스폰 지점 설정
		Type = rand() % 4;

		switch (Type)
		{
		case 0:
			Monster->SetPosition((float)m_Map->GetRect().left + 30.0f, RandF((float)m_Map->GetRect().top + 30.0f, (float)m_Map->GetRect().bottom - 30.0f));
			break;
		case 1:
			Monster->SetPosition((float)m_Map->GetRect().right - 30.0f, RandF((float)m_Map->GetRect().top + 30.0f, (float)m_Map->GetRect().bottom - 30.0f));
			break;
		case 2:
			Monster->SetPosition(RandF((float)m_Map->GetRect().left + 30.0f, (float)m_Map->GetRect().right - 30.0f), (float)m_Map->GetRect().top + 30.0f);
			break;
		case 3:
			Monster->SetPosition(RandF((float)m_Map->GetRect().left + 30.0f, (float)m_Map->GetRect().right - 30.0f), (float)m_Map->GetRect().bottom - 30.0f);
			break;
		}

		// 맵의 중앙을 도착점으로 지정
		Monster->SetDirect((float)m_Map->GetRect().right * 0.5f - Monster->GetPosition().m_X, (float)m_Map->GetRect().bottom * 0.5f - Monster->GetPosition().m_Y);
		Monster->SetLength(sqrtf(powf((float)Monster->GetDirect().x, 2) + powf((float)Monster->GetDirect().y, 2)));

		m_Monsters.push_back(Monster);
	}
}

void CGameScene::Animate(float DeltaTime)
{
	if (m_Tower)
	{
		m_Tower->Animate(DeltaTime);
	}

	if (m_Player)
	{
		m_Player->Animate(DeltaTime);
		m_Player->UpdateCamera(m_ClientRect, m_Map->GetRect());
	}

	for (int i = 0; i < m_Monsters.size(); i++)
	{
		if (m_Monsters[i])
		{
			m_Monsters[i]->Animate(DeltaTime);
		}
	}

	CreateMonster(DeltaTime);

	CheckTowerByMonsterCollision();
	CheckBulletByMonsterCollision();
}

void CGameScene::Render(HDC hDC, HDC hMemDC, HDC hMemDC2)
{
	m_hBitmap = CreateCompatibleBitmap(hDC, m_Map->GetRect().right, m_Map->GetRect().bottom);
	m_hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

	SelectObject(hMemDC2, CFileManager::GetInstance()->GetBitmap("Background"));
	m_Map->Render(hMemDC, hMemDC2);

	SelectObject(hMemDC2, CFileManager::GetInstance()->GetBitmap("SpriteSheet"));
	m_Tower->Render(hMemDC, hMemDC2);
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

void CGameScene::CreateMonster(float DeltaTime)
{
	m_CurrentGenTime += DeltaTime;

	if (m_CurrentGenTime >= m_MonsterGenTime)
	{
		for (const auto& Monster : m_Monsters)
		{
			if (!Monster->IsActive())
			{
				Monster->SetActive(true);
				Monster->SetHp(10 * Monster->GetType());

				switch (Monster->GetType())
				{
				case 0:
					Monster->SetPosition((float)m_Map->GetRect().left + 30.0f, RandF((float)m_Map->GetRect().top + 30.0f, (float)m_Map->GetRect().bottom - 30.0f));
					break;
				case 1:
					Monster->SetPosition((float)m_Map->GetRect().right - 30.0f, RandF((float)m_Map->GetRect().top + 30.0f, (float)m_Map->GetRect().bottom - 30.0f));
					break;
				case 2:
					Monster->SetPosition(RandF((float)m_Map->GetRect().left + 30.0f, (float)m_Map->GetRect().right - 30.0f), (float)m_Map->GetRect().top + 30.0f);
					break;
				case 3:
					Monster->SetPosition(RandF((float)m_Map->GetRect().left + 30.0f, (float)m_Map->GetRect().right - 30.0f), (float)m_Map->GetRect().bottom - 30.0f);
				}

				Monster->SetDirect((float)m_Map->GetRect().right * 0.5f - Monster->GetPosition().m_X, (float)m_Map->GetRect().bottom * 0.5f - Monster->GetPosition().m_Y);
				Monster->SetLength(sqrtf(powf((float)Monster->GetDirect().x, 2) + powf((float)Monster->GetDirect().y, 2)));

				printf("(%.03f, %.03f)에 몬스터 생성됨!\n", Monster->GetPosition().m_X, Monster->GetPosition().m_Y);
				break;
			}
		}
		m_CurrentGenTime = 0.0f;
	}
}

void CGameScene::CheckTowerByMonsterCollision()
{
	if (m_Tower->IsActive())
	{
		RECT CollidedRect{};
		RECT TowerRect{ (int)(m_Tower->GetPosition().m_X - 0.5f * m_Tower->GetWidth()),
						(int)(m_Tower->GetPosition().m_Y - 0.5f * m_Tower->GetHeight()),
						(int)(m_Tower->GetPosition().m_X + 0.5f * m_Tower->GetWidth()),
						(int)(m_Tower->GetPosition().m_Y + 0.5f * m_Tower->GetHeight()) };

		for (const auto& Monster : m_Monsters)
		{
			if (Monster->IsActive())
			{
				RECT MonsterRect{ (int)(Monster->GetPosition().m_X - 0.5f * Monster->GetWidth()),
								 (int)(Monster->GetPosition().m_Y - 0.5f * Monster->GetHeight()),
								 (int)(Monster->GetPosition().m_X + 0.5f * Monster->GetWidth()),
								 (int)(Monster->GetPosition().m_Y + 0.5f * Monster->GetHeight()) };

				if (IntersectRect(&CollidedRect, &TowerRect, &MonsterRect))
				{
					m_Tower->SetHp(m_Tower->GetHp() - 10 * Monster->GetType());
				}
			}
		}
	}
}

void CGameScene::CheckBulletByMonsterCollision()
{
	CBullet* Bullets{ m_Player->GetBullets() };
	RECT CollidedRect{};

	for (int i = 0; i < MAX_BULLET; ++i)
	{
		if (Bullets[i].IsActive())
		{
			RECT BulletRect{ (int)(Bullets[i].GetPosition().m_X - 0.5f * Bullets[i].GetWidth()),
							 (int)(Bullets[i].GetPosition().m_Y - 0.5f * Bullets[i].GetHeight()),
							 (int)(Bullets[i].GetPosition().m_X + 0.5f * Bullets[i].GetWidth()),
							 (int)(Bullets[i].GetPosition().m_Y + 0.5f * Bullets[i].GetHeight()) };

			for (int j = 0; j < m_Monsters.size(); ++j)
			{
				if (m_Monsters[j]->IsActive())
				{
					RECT MonsterRect{ (int)(m_Monsters[j]->GetPosition().m_X - 0.5f * m_Monsters[j]->GetWidth()),
									  (int)(m_Monsters[j]->GetPosition().m_Y - 0.5f * m_Monsters[j]->GetHeight()),
									  (int)(m_Monsters[j]->GetPosition().m_X + 0.5f * m_Monsters[j]->GetWidth()),
									  (int)(m_Monsters[j]->GetPosition().m_Y + 0.5f * m_Monsters[j]->GetHeight()) };

					if (IntersectRect(&CollidedRect, &BulletRect, &MonsterRect))
					{
						Bullets[i].SetActive(false);
						m_Monsters[j]->SetHp(m_Monsters[j]->GetHp() - 10);
					}
				}
			}
		}
	}
}
