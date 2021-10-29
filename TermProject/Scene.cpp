#include "stdafx.h"
#include "Scene.h"
#include "Map.h"
#include "Player.h"

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
	GetClientRect(hWnd, &m_ClientRect);
	BuildObject();
}

void CGameScene::OnDestroy()
{

}

void CGameScene::BuildObject()
{
	RECT MapRect{ 0.0f, 0.0f, 2000.0f, 1000.0f };

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

void CGameScene::Render(const HDC& hDC, const HDC& hMemDC)
{
	RECT MapRect{ m_Map->GetRect() };

	m_hBitmap = CreateCompatibleBitmap(hDC, MapRect.right, MapRect.bottom);
	m_hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

	// 그리기
	PatBlt(hMemDC, MapRect.left, MapRect.top, MapRect.right, MapRect.bottom, WHITENESS);

	// 맵 사각형
	Rectangle(hMemDC, MapRect.left, MapRect.top, MapRect.right, MapRect.bottom);

	// 주인공 사각형
	DrawRectangle(hMemDC, m_Player->GetPosition(), m_Player->GetWidth(), m_Player->GetHeight());
	Ellipse(hMemDC, 375, 275, 425, 325);
	Ellipse(hMemDC, 675, 275, 725, 325);
	Ellipse(hMemDC, 975, 275, 1025, 325);
	Ellipse(hMemDC, 1275, 275, 1325, 325);
	Ellipse(hMemDC, 1575, 275, 1625, 325);

	Ellipse(hMemDC, 375, 475, 425, 525);
	Ellipse(hMemDC, 675, 675, 725, 725);
	Ellipse(hMemDC, 975, 875, 1025, 925);
	Ellipse(hMemDC, 1275, 1075, 1325, 1175);
	Ellipse(hMemDC, 1575, 1275, 1625, 1375);

	POSITION PlayerCameraPos{ m_Player->GetCameraStartPosition() };

	BitBlt(hDC, 0, 0, m_ClientRect.right, m_ClientRect.bottom, hMemDC, PlayerCameraPos.m_X, PlayerCameraPos.m_Y, SRCCOPY);
	SelectObject(hMemDC, m_hOldBitmap);
	DeleteObject(m_hBitmap);
}