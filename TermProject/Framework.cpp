#include "stdafx.h"
#include "Framework.h"

CFramework::CFramework()
{
	m_Timer = new CTimer{};
}

CFramework::~CFramework()
{
	if (m_Timer)
	{
		delete m_Timer;
	}
}

void CFramework::OnCreate(const HINSTANCE& hInstance, const HWND& hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;
	m_hDC = GetDC(m_hWnd);

	m_Timer->Start();
}

void CFramework::OnDestroy()
{

}

void CFramework::ProcessKeyboardMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	default:
		break;
	}
}

void CFramework::ProcessMouseMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

void CFramework::ProcessInput()
{
	float Speed{ 100.0f * m_Timer->GetDeltaTime() };

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_PlayerRect.Top -= Speed;
		m_PlayerRect.Bottom -= Speed;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_PlayerRect.Top += Speed;
		m_PlayerRect.Bottom += Speed;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_PlayerRect.Left -= Speed;
		m_PlayerRect.Right -= Speed;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_PlayerRect.Left += Speed;
		m_PlayerRect.Right += Speed;
	}
}


void CFramework::Update()
{
	m_Timer->Update();
	ProcessInput();
	Render();
}

void CFramework::Animate()
{

}

void CFramework::Render()
{
	RECT ClientFloatRect{};

	GetClientRect(m_hWnd, &ClientFloatRect);

	m_hBackDC = CreateCompatibleDC(m_hDC);
	m_hBitmap = CreateCompatibleBitmap(m_hDC, ClientFloatRect.right, ClientFloatRect.bottom);
	m_hOldBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBitmap);

	// ±×¸®±â
	PatBlt(m_hBackDC, 0, 0, ClientFloatRect.right, ClientFloatRect.bottom, WHITENESS);
	Rectangle(m_hBackDC, m_PlayerRect.Left, m_PlayerRect.Top, m_PlayerRect.Right, m_PlayerRect.Bottom);
	
	BitBlt(m_hDC, 0, 0, ClientFloatRect.right, ClientFloatRect.bottom, m_hBackDC, 0, 0, SRCCOPY);
	SelectObject(m_hBackDC, m_hOldBitmap);
	DeleteObject(m_hBitmap);
	DeleteDC(m_hBackDC);
	InvalidateRect(m_hWnd, &ClientFloatRect, false);
}

