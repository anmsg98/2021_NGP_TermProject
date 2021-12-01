#include "stdafx.h"
#include "Scene.h"
#include "Map.h"
#include "Button.h"

// ============================================== CWaitingScene ==============================================

CWaitingScene::~CWaitingScene()
{
	if (m_Map)
	{
		delete m_Map;
	}
}

void CWaitingScene::ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

void CWaitingScene::ProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_CursorPos.x = LOWORD(lParam);
	m_CursorPos.y = HIWORD(lParam);

	switch (message)
	{
	case WM_MOUSEMOVE:
		for (int i = 0; i < MAX_BUTTON; ++i)
		{
			if (m_Buttons[i]->GetPosition().m_X - 0.5f * m_Buttons[i]->GetWidth() <= m_CursorPos.x && m_CursorPos.x <= m_Buttons[i]->GetPosition().m_X + 0.5f * m_Buttons[i]->GetWidth() &&
				m_Buttons[i]->GetPosition().m_Y - 0.5f * m_Buttons[i]->GetHeight() <= m_CursorPos.y && m_CursorPos.y <= m_Buttons[i]->GetPosition().m_Y + 0.5f * m_Buttons[i]->GetHeight())
			{
				if (!m_Buttons[i]->IsActive())
				{
					CSoundManager::GetInstance()->Play(CSoundManager::BUTTON_SOUND, 0.6f);
				}

				m_Buttons[i]->SetActive(true);
			}
			else
			{
				m_Buttons[i]->SetActive(false);
			}
		}
		break;
	}
}

void CWaitingScene::ProcessInput()
{
	if (GetAsyncKeyState(MK_LBUTTON) & 0x0001)
	{
		for (int i = 0; i < MAX_BUTTON; ++i)
		{
			if (m_Buttons[i]->IsActive())
			{
				switch (m_Buttons[i]->GetType())
				{
				case CButton::READY:
					if (m_GameData->m_Players[m_ID].IsReady())
					{
						m_GameData->m_Players[m_ID].SetReady(false);
					}
					else
					{
						m_GameData->m_Players[m_ID].SetReady(true);
					}
					break;
				case CButton::EXIT:
					PostQuitMessage(0);
					break;
				}
				break;
			}
		}
	}
}

void CWaitingScene::OnCreate(HINSTANCE hInstance, HWND hWnd, int ID, GameData* Data)
{
	GetClientRect(hWnd, &m_ClientRect);
	BuildObject(ID, Data);
}

void CWaitingScene::OnDestroy()
{

}

void CWaitingScene::BuildObject(int ID, GameData* Data)
{
	m_ID = ID;
	m_GameData = Data;

	// 맵을 생성한다.
	RECT MapRect{ 0, 0, 800, 600 };

	m_Map = new CMap{};
	m_Map->SetRect(MapRect);

	// 버튼을 생성한다.
	m_Buttons[0] = new CButton{};
	m_Buttons[0]->SetActive(false);
	m_Buttons[0]->SetType(CButton::READY);
	m_Buttons[0]->SetSize(VECTOR2D(300.0f, 62.0f));
	m_Buttons[0]->SetPosition(0.5f * m_Buttons[0]->GetWidth() + 250.0f, 0.5f * m_Buttons[0]->GetHeight() + 330.0f);

	m_Buttons[1] = new CButton{};
	m_Buttons[1]->SetActive(false);
	m_Buttons[1]->SetType(CButton::EXIT);
	m_Buttons[1]->SetSize(VECTOR2D(300.0f, 62.0f));
	m_Buttons[1]->SetPosition(0.5f * m_Buttons[1]->GetWidth() + 250.0f, 0.5f * m_Buttons[1]->GetHeight() + 410.0f);
}

void CWaitingScene::Render(HDC hDC, HDC hMemDC, HDC hMemDC2)
{
	m_hBitmap = CreateCompatibleBitmap(hDC, m_Map->GetRect().right, m_Map->GetRect().bottom);
	m_hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

	HBITMAP hOldBitmap{};

	hOldBitmap = (HBITMAP)SelectObject(hMemDC2, CFileManager::GetInstance()->GetBitmap("BACKGROUND_W"));
	m_Map->Render(hMemDC, hMemDC2);
	SelectObject(hMemDC2, hOldBitmap);

	hOldBitmap = (HBITMAP)SelectObject(hMemDC2, CFileManager::GetInstance()->GetBitmap("SPRITE_SHEET"));
	m_Buttons[0]->Render(hMemDC, hMemDC2);
	m_Buttons[1]->Render(hMemDC, hMemDC2);

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		SelectObject(hMemDC2, hOldBitmap);
		m_GameData->m_Players[i].Render(hMemDC, hMemDC2);
	}

	BitBlt(hDC, 0, 0, m_ClientRect.right, m_ClientRect.bottom, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, m_hOldBitmap);
	DeleteObject(m_hBitmap);
}

// ============================================== CGameScene ==============================================

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
}

void CGameScene::ProcessInput()
{
	VECTOR2D Position{ m_GameData->m_Players[m_ID].GetPosition() };
	float Speed{ 3.0f };

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

	hOldBitmap = (HBITMAP)SelectObject(hMemDC2, CFileManager::GetInstance()->GetBitmap("BACKGROUND_I"));
	m_Map->Render(hMemDC, hMemDC2);
	SelectObject(hMemDC2, hOldBitmap);

	hOldBitmap = (HBITMAP)SelectObject(hMemDC2, CFileManager::GetInstance()->GetBitmap("SPRITE_SHEET"));
	m_GameData->m_Tower.Render(hMemDC, hMemDC2);

	for (int i = 0; i < MAX_MONSTER; ++i)
	{
		m_GameData->m_Monsters[i].Render(hMemDC, hMemDC2);
	}

	for (int i = 0; i < MAX_ITEM; ++i)
	{
		m_GameData->m_Items[i].Render(hMemDC, hMemDC2);
	}

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		SelectObject(hMemDC2, hOldBitmap);
		m_GameData->m_Players[i].Render(hMemDC, hMemDC2);
	}

	POINT PlayerCameraPos{ m_GameData->m_Players[m_ID].GetCameraStartPosition() };

	BitBlt(hDC, 0, 0, m_ClientRect.right, m_ClientRect.bottom, hMemDC, PlayerCameraPos.x, PlayerCameraPos.y, SRCCOPY);
	SelectObject(hMemDC, m_hOldBitmap);
	DeleteObject(m_hBitmap);
}
