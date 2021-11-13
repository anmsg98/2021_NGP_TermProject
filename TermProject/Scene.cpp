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
	m_GameData->m_Players[m_ID].SetDirect(m_CursorPos.x + m_GameData->m_Players[m_ID].GetCameraStartPosition().x - m_GameData->m_Players[m_ID].GetPosition().m_X,
										  m_CursorPos.y + m_GameData->m_Players[m_ID].GetCameraStartPosition().y - m_GameData->m_Players[m_ID].GetPosition().m_Y);

	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		m_GameData->m_Players[m_ID].FireBullet(m_CursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	}
}

void CGameScene::ProcessInput()
{
	float Speed{ 300.0f * m_GameData->m_DeltaTime };
	RECT MapRect{ m_Map->GetRect() };
	POSITION PlayerPos{ m_GameData->m_Players[m_ID].GetPosition() };

	if (GetAsyncKeyState('W') & 0x8000)
	{
		if (PlayerPos.m_Y > MapRect.top + 0.5f * m_GameData->m_Players[m_ID].GetHeight())
		{
			PlayerPos.m_Y -= Speed;
		}
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		if (PlayerPos.m_Y < MapRect.bottom - 0.5f * m_GameData->m_Players[m_ID].GetHeight())
		{
			PlayerPos.m_Y += Speed;
		}
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		if (PlayerPos.m_X > MapRect.left + 0.5f * m_GameData->m_Players[m_ID].GetWidth())
		{
			PlayerPos.m_X -= Speed;
		}
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		if (PlayerPos.m_X < MapRect.right - 0.5f * m_GameData->m_Players[m_ID].GetWidth())
		{
			PlayerPos.m_X += Speed;
		}
	}

	m_GameData->m_Players[m_ID].SetPosition(PlayerPos);
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
