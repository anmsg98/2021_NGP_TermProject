#pragma once
#include "Timer.h"

class CScene;

class CFramework
{
private:
	bool						m_IsActive{};

	PAINTSTRUCT					ps;

	RECT						m_ClientRect{};

	HINSTANCE					m_hInstance{};
	HWND						m_hWnd{};

	HDC							m_hDC{};
	HDC							m_hMemDC{};
	HDC							m_MemDC{};

	CTimer*						m_Timer{};

	stack<CScene*>				m_Scenes{};

public:
	CFramework();
	~CFramework();

	void OnCreate(const HINSTANCE& hInstance, const HWND& hWnd);
	void OnDestroy();

	void ProcessKeyboardMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ProcessMouseMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ProcessInput();

	void Update();
	void Animate();
	void Render();
};
