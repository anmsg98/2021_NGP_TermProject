#pragma once

class CMap;
class CPlayer;

class CScene
{
protected:
	RECT				m_ClientRect{};

	// 더블버퍼링용 비트맵
	HBITMAP				m_hBitmap{};
	HBITMAP				m_hOldBitmap{};

public:
	CScene() = default;
	virtual ~CScene() = default;

	virtual void OnCreate(const HINSTANCE& hInstance, const HWND& hWnd) = 0;
	virtual void OnDestroy() = 0;
 
	virtual void ProcessKeyboardMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessMouseMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessInput(float DeltaTime) = 0;

	virtual void BuildObject() = 0;

	virtual void Update() = 0;
	virtual void Animate(float DeltaTime) = 0;
	virtual void Render(const HDC& hDC, const HDC& hBackDC) = 0;
};

class CGameScene : public CScene
{
private:
	CMap*				m_Map{};
	CPlayer*			m_Player{};

public:
	CGameScene() = default;
	virtual ~CGameScene();

	virtual void OnCreate(const HINSTANCE& hInstance, const HWND& hWnd);
	virtual void OnDestroy();

	virtual void BuildObject();

	virtual void ProcessKeyboardMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessMouseMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessInput(float DeltaTime);

	virtual void Update();
	virtual void Animate(float DeltaTime);
	virtual void Render(const HDC& hDC, const HDC& hBackDC);
};
