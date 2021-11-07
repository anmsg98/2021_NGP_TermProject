#include "stdafx.h"
#include "Scene.h"
#include "Map.h"
#include "Player.h"
#include "Monster.h"

CFileManager* CScene::m_FileManager;

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

CFileManager* CScene::GetFileManager()
{
	if (!m_FileManager)
	{
		m_FileManager = new CFileManager{};
		m_FileManager->SetImageRectFromFile("Image/SpriteCoord.txt");
	}

	return m_FileManager;
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
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (m_Player)
		{
			m_Player->FireBullet(mx, my);
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
	m_BgBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_BG));
	m_PlayerBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_PLAYER));
	
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
	for (int i = 0; i < m_Monsters.size(); i++) {
		if (m_Monsters[i])
		{
			if (m_Monsters[i]->IsActive())
			{
				m_Monsters[i]->Animate(DeltaTime);
			}
		}
	}

	CreateMonster(DeltaTime);
	CheckMonsterByBulletCollisions();
}


void CGameScene::CreateMonster(float DeltaTime)
{
	CreateMonsterTimeElapsed += DeltaTime;
	if (CreateMonsterTimeElapsed > CreateMonsterCycle)
	{
		CMonster* Monster = new CMonster();

		Monster->SetActive(true);
		Monster->SetWidth(73.5f);
		Monster->SetHeight(76.0f);

		// 타입결정
		int random = rand() % 3 + 1;
		Monster->SetType(random);

		//생성지점 결정
		random = rand() % 4;
		if (random == 0)
		{
			Monster->SetPosition(m_Map->GetRect().left, RandF2(m_Map->GetRect().top, m_Map->GetRect().bottom));
		}
		else if (random == 1)
		{
			Monster->SetPosition(m_Map->GetRect().right, RandF2(m_Map->GetRect().top, m_Map->GetRect().bottom));
		}
		else if (random == 2)
		{
			Monster->SetPosition(RandF2(m_Map->GetRect().left, m_Map->GetRect().right), m_Map->GetRect().top);
		}
		else if (random == 3)
		{
			Monster->SetPosition(RandF2(m_Map->GetRect().left, m_Map->GetRect().right), m_Map->GetRect().bottom);
		}

		//맵의 중앙을 도착점으로 지정
		RECT MapRect{ m_Map->GetRect() };

		Monster->SetDirect(MapRect.right * 0.5f - Monster->GetPosition().m_X, MapRect.bottom * 0.5f - Monster->GetPosition().m_Y);
		Monster->SetLength(sqrt(pow(Monster->GetDirect().x, 2) + pow(Monster->GetDirect().y, 2)));

		m_Monsters.push_back(Monster);
		CreateMonsterTimeElapsed = 0.0f;
	}
}

void CGameScene::CheckMonsterByBulletCollisions()
{
	CBullet* Bullets{ m_Player->GetBullets() };

	for (int i = 0; i < MAX_BULLET; ++i)
	{
		for (int j = 0; j < m_Monsters.size(); ++j)
		{

			if (Bullets[i].IsActive() && m_Monsters[j]->IsActive())
			{
				RECT tmp{};
				RECT Bullet{ Bullets[i].GetPosition().m_X - 0.5f * Bullets[i].GetWidth(), Bullets[i].GetPosition().m_Y - 0.5f * Bullets[i].GetHeight(),
				             Bullets[i].GetPosition().m_X + 0.5f * Bullets[i].GetWidth(), Bullets[i].GetPosition().m_Y + 0.5f * Bullets[i].GetHeight()};
				RECT Monster{ m_Monsters[j]->GetPosition().m_X - 0.5f * m_Monsters[j]->GetWidth(), m_Monsters[j]->GetPosition().m_Y - 0.5f * m_Monsters[j]->GetHeight(),
							  m_Monsters[j]->GetPosition().m_X + 0.5f * m_Monsters[j]->GetWidth(), m_Monsters[j]->GetPosition().m_Y + 0.5f * m_Monsters[j]->GetHeight() };
				if (IntersectRect(&tmp, &Bullet, &Monster))
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
	RECT MapRect{ m_Map->GetRect() };

	m_hBitmap = CreateCompatibleBitmap(hDC, MapRect.right, MapRect.bottom);
	m_hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

	// 배경
	LTWH ltwh{ GetFileManager()->GetImageRect("Background") };

	SelectObject(hMemDC2, m_BgBitmap);
	BitBlt(hMemDC, ltwh.m_Left, ltwh.m_Top, ltwh.m_Width, ltwh.m_Height, hMemDC2, 0, 0, SRCCOPY);
	
	// 플레이어 
	ltwh = GetFileManager()->GetImageRect("Player_1");

	SelectObject(hMemDC2, m_PlayerBitmap);
	TransparentBlt(hMemDC, m_Player->GetPosition().m_X - 0.5f * m_Player->GetWidth(), m_Player->GetPosition().m_Y - 0.5f * m_Player->GetHeight(), m_Player->GetWidth(), m_Player->GetHeight(),
				   hMemDC2, ltwh.m_Left, ltwh.m_Top, ltwh.m_Width, ltwh.m_Height, GetFileManager()->GetTransColor());
	
	// 총알
	ltwh = GetFileManager()->GetImageRect("Bullet_1");

	CBullet* Bullets{ m_Player->GetBullets() };

	for (int i = 0; i < MAX_BULLET; ++i)
	{
		if (Bullets[i].IsActive())
		{
			TransparentBlt(hMemDC, Bullets[i].GetPosition().m_X - 0.5f * Bullets[i].GetWidth(), Bullets[i].GetPosition().m_Y - 0.5f * Bullets[i].GetHeight(), Bullets[i].GetWidth(), Bullets[i].GetHeight(),
						   hMemDC2, ltwh.m_Left, ltwh.m_Top, ltwh.m_Width, ltwh.m_Height, GetFileManager()->GetTransColor());
		}
	}

	//몬스터	

	for (int i = 0; i < m_Monsters.size(); ++i) {
		if (m_Monsters[i]->IsActive())
		{
			int Type = m_Monsters[i]->GetType();
			if(Type == 1)
			{
				ltwh = GetFileManager()->GetImageRect("Monster_1_1");
				TransparentBlt(hMemDC, m_Monsters[i]->GetPosition().m_X - 0.5f * m_Monsters[i]->GetWidth(), m_Monsters[i]->GetPosition().m_Y - 0.5f * m_Monsters[i]->GetHeight(), m_Monsters[i]->GetWidth(), m_Monsters[i]->GetHeight(),
					hMemDC2, ltwh.m_Left, ltwh.m_Top, ltwh.m_Width, ltwh.m_Height, GetFileManager()->GetTransColor());
			}
			else if (Type == 2)
			{
				ltwh = GetFileManager()->GetImageRect("Monster_2_1");
				TransparentBlt(hMemDC, m_Monsters[i]->GetPosition().m_X - 0.5f * m_Monsters[i]->GetWidth(), m_Monsters[i]->GetPosition().m_Y - 0.5f * m_Monsters[i]->GetHeight(), m_Monsters[i]->GetWidth(), m_Monsters[i]->GetHeight(),
					hMemDC2, ltwh.m_Left, ltwh.m_Top, ltwh.m_Width, ltwh.m_Height, GetFileManager()->GetTransColor());
			}
			else if (Type == 3)
			{
				ltwh = GetFileManager()->GetImageRect("Monster_3_1");
				TransparentBlt(hMemDC, m_Monsters[i]->GetPosition().m_X - 0.5f * m_Monsters[i]->GetWidth(), m_Monsters[i]->GetPosition().m_Y - 0.5f * m_Monsters[i]->GetHeight(), m_Monsters[i]->GetWidth(), m_Monsters[i]->GetHeight(),
					hMemDC2, ltwh.m_Left, ltwh.m_Top, ltwh.m_Width, ltwh.m_Height, GetFileManager()->GetTransColor());
			}
		}
	}


	POINT PlayerCameraPos{ m_Player->GetCameraStartPosition() };

	BitBlt(hDC, 0, 0, m_ClientRect.right, m_ClientRect.bottom, hMemDC, PlayerCameraPos.x, PlayerCameraPos.y, SRCCOPY);
	SelectObject(hMemDC, m_hBitmap);
	DeleteObject(m_hBitmap);
}
