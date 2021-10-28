#pragma once
#include "Timer.h"

class CFramework
{
private:
	bool						m_IsActive{};

	int							m_ClientWidth{ CLIENT_WIDTH };
	int							m_ClientHeight{ CLIENT_HEIGHT };

	HINSTANCE					m_hInstance{};
	HWND						m_hWnd{};

	HDC							m_hDC{};
	HDC							m_hBackDC{};

	HBITMAP                     m_hBitmap{};
	HBITMAP                     m_hOldBitmap{};

	CTimer*						m_Timer{};

	// test
	Rect						m_PlayerRect{ 50, 50, 100, 100 };

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
