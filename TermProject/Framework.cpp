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

}

void CFramework::ProcessMouseMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

}

void CFramework::ProcessInput()
{
	float Speed{ 100 * m_Timer->GetDeltaTime() };

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
	m_hBackDC = BeginPaint(m_hWnd, &m_Ps);
	GetClientRect(m_hWnd, &m_Crt);
	m_hBackDC = CreateCompatibleDC(m_hDC);
	m_hBmp = CreateCompatibleBitmap(m_hDC, m_Crt.right, m_Crt.bottom);
	m_hOldbmp = (HBITMAP)SelectObject(m_hBackDC, m_hBmp);

	// ±×¸®±â
	DrawObject(m_hBackDC, m_hBmp, m_hOldbmp, m_Crt);

	GetClientRect(m_hWnd, &m_Crt);
	BitBlt(m_hDC, 0, 0, m_Crt.right, m_Crt.bottom, m_hBackDC, 0, 0, SRCCOPY);
	SelectObject(m_hBackDC, m_hOldbmp);
	DeleteObject(m_hBmp);
	DeleteDC(m_hBackDC);
	EndPaint(m_hWnd, &m_Ps);
	InvalidateRect(m_hWnd, &m_Crt, false);
}

void CFramework::DrawObject(HDC hDCBuff, HBITMAP hBMPBuff, HBITMAP hBMPBuffOld, RECT Crt)
{
	PatBlt(hDCBuff, 0, 0, Crt.right, Crt.bottom, WHITENESS);
	Rectangle(hDCBuff, m_PlayerRect.Left, m_PlayerRect.Top, m_PlayerRect.Right, m_PlayerRect.Bottom);
}
