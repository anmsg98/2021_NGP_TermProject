#include "stdafx.h"
#include "Framework.h"
#include "Scene.h"

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

	GetClientRect(hWnd, &m_ClientRect);

	// 씬을 생성한다.
	CGameScene* Scene{ new CGameScene{} };

	Scene->OnCreate(hInstance, hWnd);
	m_Scenes.push(Scene);
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
		m_Scenes.top()->ProcessMouseMessage(hWnd, message, wParam, lParam);
		break;
	}
}

void CFramework::ProcessInput()
{
	m_Scenes.top()->ProcessInput(m_Timer->GetDeltaTime());
}

void CFramework::Animate()
{
	m_Scenes.top()->Animate(m_Timer->GetDeltaTime());
}

void CFramework::PrepareRender()
{
	InvalidateRect(m_hWnd, &m_ClientRect, false);
	UpdateWindow(m_hWnd);
}

void CFramework::Render()
{
	m_hDC = BeginPaint(m_hWnd, &m_PaintStruct);
	m_hMemDC = CreateCompatibleDC(m_hDC);
	m_hMemDC2 = CreateCompatibleDC(m_hMemDC);

	m_Scenes.top()->Render(m_hDC, m_hMemDC, m_hMemDC2);

	DeleteDC(m_hMemDC);
	DeleteDC(m_hMemDC2);	
	EndPaint(m_hWnd, &m_PaintStruct);
}

void CFramework::Update()
{
	m_Timer->Update();

	ProcessInput();
	Animate();
	PrepareRender();
}
