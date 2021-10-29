#include "stdafx.h"
#include "Scene.h"
#include "Map.h"
#include "Player.h"
#include "Resource.h"
#pragma comment (lib, "Msimg32.lib")

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
}

void CGameScene::OnCreate(const HINSTANCE& hInstance, const HWND& hWnd)
{
	m_Bg_Bitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_BG));
	m_Player_Bitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_PLAYER));
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
	m_Player->SetWidth(30.0f);
	m_Player->SetHeight(30.0f);
}

void CGameScene::ProcessKeyboardMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

void CGameScene::ProcessMouseMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		break;
	}
}

void CGameScene::ProcessInput(float DeltaTime)
{
	float Speed{ 300.0f * DeltaTime };
	RECT MapRect{ m_Map->GetRect() };
	POSITION PlayerPos{ m_Player->GetPosition() };

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (PlayerPos.m_Y > MapRect.top + 0.5f * m_Player->GetHeight())
		{
			PlayerPos.m_Y -= Speed;
		}
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (PlayerPos.m_Y < MapRect.bottom - 0.5f * m_Player->GetHeight())
		{
			PlayerPos.m_Y += Speed;
		}
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		if (PlayerPos.m_X > MapRect.left + 0.5f * m_Player->GetWidth())
		{
			PlayerPos.m_X -= Speed;
		}
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		if (PlayerPos.m_X < MapRect.right - 0.5f * m_Player->GetWidth())
		{
			PlayerPos.m_X += Speed;
		}
	}

	m_Player->SetPosition(PlayerPos);
}

void CGameScene::Update()
{
	if (m_Player)
	{
		m_Player->Update(m_ClientRect, m_Map->GetRect());
	}
}

void CGameScene::Animate(float DeltaTime)
{

}

void CGameScene::Render(const HDC& hDC, const HDC& hMemDC, const HDC& MemDC)
{
	RECT MapRect{ m_Map->GetRect() };

	m_hBitmap = CreateCompatibleBitmap(hDC, MapRect.right, MapRect.bottom);
	m_hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

	// 배경
	SelectObject(MemDC, m_Bg_Bitmap);
	StretchBlt(hMemDC, 0,0, 2400, 1500, MemDC, 0, 0, 2400, 1500, SRCCOPY);
	
	// 맵을 그리는 위치

	// 주인공 
	SelectObject(MemDC, m_Player_Bitmap);
	TransparentBlt(hMemDC, m_Player->GetPosition().m_X, m_Player->GetPosition().m_Y, 125, 180, MemDC, 0, 0, 125, 180, RGB(255, 174, 201));
	/*Ellipse(hMemDC, 375, 275, 425, 325);
	Ellipse(hMemDC, 675, 275, 725, 325);
	Ellipse(hMemDC, 975, 275, 1025, 325);
	Ellipse(hMemDC, 1275, 275, 1325, 325);
	Ellipse(hMemDC, 1575, 275, 1625, 325);
	Ellipse(hMemDC, 375, 675, 425, 725);
	Ellipse(hMemDC, 675, 675, 725, 725);
	Ellipse(hMemDC, 975, 675, 1025, 725);
	Ellipse(hMemDC, 1275, 675, 1325, 725);
	Ellipse(hMemDC, 1575, 675, 1625, 725);*/

	POINT PlayerCameraPos{ m_Player->GetCameraStartPosition() };

	BitBlt(hDC, 0, 0, m_ClientRect.right, m_ClientRect.bottom, hMemDC, PlayerCameraPos.x, PlayerCameraPos.y, SRCCOPY);
	SelectObject(hMemDC, m_hBitmap);
	DeleteObject(m_hBitmap);
}
