#include "stdafx.h"
#include "Scene.h"
#include "Map.h"

CGameScene::~CGameScene()
{
	if (m_Map)
	{
		delete m_Map;
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
	m_CursorPos.x = LOWORD(lParam);
	m_CursorPos.y = HIWORD(lParam);

	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		break;
	}
}

void CGameScene::ProcessInput()
{
	POSITION Position{ m_GameData->m_Players[m_ID].GetPosition() };
	float Speed{ 300.0f * m_GameData->m_DeltaTime };

	// 플레이어의 위치 값 조정
	if (GetAsyncKeyState('W') & 0x8000)
	{
		if (Position.m_Y > m_Map->GetRect().top + 0.5f * m_GameData->m_Players[m_ID].GetHeight())
		{
			Position.m_Y -= Speed;
		}
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		if (Position.m_Y < m_Map->GetRect().bottom - 0.5f * m_GameData->m_Players[m_ID].GetHeight())
		{
			Position.m_Y += Speed;
		}
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		if (Position.m_X > m_Map->GetRect().left + 0.5f * m_GameData->m_Players[m_ID].GetWidth())
		{
			Position.m_X -= Speed;
		}
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		if (Position.m_X < m_Map->GetRect().right - 0.5f * m_GameData->m_Players[m_ID].GetWidth())
		{
			Position.m_X += Speed;
		}
	}

	// 플레이어의 총알 발사 처리
	if (GetAsyncKeyState(MK_LBUTTON) & 0x0001)
	{
		m_GameData->m_Players[m_ID].FireBullet(m_CursorPos);
	}

	// 플레이어의 위치, 방향, 카메라 등 갱신
	m_GameData->m_Players[m_ID].SetPosition(Position);
	m_GameData->m_Players[m_ID].SetDirection(m_CursorPos.x + m_GameData->m_Players[m_ID].GetCameraStartPosition().x - m_GameData->m_Players[m_ID].GetPosition().m_X,
											 m_CursorPos.y + m_GameData->m_Players[m_ID].GetCameraStartPosition().y - m_GameData->m_Players[m_ID].GetPosition().m_Y);
	m_GameData->m_Players[m_ID].UpdateCamera(m_ClientRect, m_Map->GetRect());
}

void CGameScene::OnCreate(HINSTANCE hInstance, HWND hWnd, int ID, GameData* Data)
{
	CFileManager::GetInstance()->LoadBitmaps(hInstance);
	CFileManager::GetInstance()->LoadRectFromFile("Image/SpriteCoord.txt");

	GetClientRect(hWnd, &m_ClientRect);
	BuildObject(ID, Data);
}

void CGameScene::OnDestroy()
{

}

void CGameScene::BuildObject(int ID, GameData* Data)
{
	m_ID = ID;
	m_GameData = Data;

	// 맵을 생성한다.
	RECT MapRect{ 0, 0, 2400, 1500 };

	m_Map = new CMap{};
	m_Map->SetRect(MapRect);
}

void CGameScene::Animate()
{
	m_GameData->m_Tower.Animate(m_GameData->m_DeltaTime);

	for (int i = 0; i < MAX_MONSTER; ++i)
	{
		m_GameData->m_Monsters[i].Animate(m_GameData->m_DeltaTime);
	}

	for (int i = 0; i < MAX_ITEM; ++i)
	{
		m_GameData->m_Items[i].Animate(m_GameData->m_DeltaTime);
	}

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		m_GameData->m_Players[i].Animate(m_GameData->m_DeltaTime);
	}
}

void CGameScene::Render(HDC hDC, HDC hMemDC, HDC hMemDC2)
{
	m_hBitmap = CreateCompatibleBitmap(hDC, m_Map->GetRect().right, m_Map->GetRect().bottom);
	m_hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

	HBITMAP hOldBitmap{};

	hOldBitmap = (HBITMAP)SelectObject(hMemDC2, CFileManager::GetInstance()->GetBitmap("Background"));
	m_Map->Render(hMemDC, hMemDC2);
	SelectObject(hMemDC2, hOldBitmap);

	hOldBitmap = (HBITMAP)SelectObject(hMemDC2, CFileManager::GetInstance()->GetBitmap("SpriteSheet"));
	m_GameData->m_Tower.Render(hMemDC, hMemDC2);

	for (int i = 0 ; i < MAX_MONSTER; ++i)
	{
		m_GameData->m_Monsters[i].Render(hMemDC, hMemDC2);
	}

	for (int i = 0 ; i < MAX_ITEM ; ++i)
	{
		m_GameData->m_Items[i].Render(hMemDC, hMemDC2);
	}

	SelectObject(hMemDC2, hOldBitmap);

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		m_GameData->m_Players[i].Render(hMemDC, hMemDC2);
	}

	POINT PlayerCameraPos{ m_GameData->m_Players[m_ID].GetCameraStartPosition() };

	BitBlt(hDC, 0, 0, m_ClientRect.right, m_ClientRect.bottom, hMemDC, PlayerCameraPos.x, PlayerCameraPos.y, SRCCOPY);
	SelectObject(hMemDC, m_hOldBitmap);
	DeleteObject(m_hBitmap);
}
