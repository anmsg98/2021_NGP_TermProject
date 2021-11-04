#pragma once
#include "FileManager.h"

class CMap;
class CPlayer;

class CScene
{
private:
	static CFileManager*			m_FileManager;

protected:
	RECT							m_ClientRect{};

	// ´õºí¹öÆÛ¸µ¿ë ºñÆ®¸Ê
	HBITMAP							m_hBitmap{};
	HBITMAP							m_hOldBitmap{};
	
	// °´Ã¼ ºñÆ®¸Ê
	HBITMAP							m_BgBitmap{};
	HBITMAP							m_PlayerBitmap{};

public:
	CScene() = default;
	virtual ~CScene() = default;

	CFileManager* GetFileManager();

	virtual void ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessInput(float DeltaTime) = 0;

	virtual void OnCreate(HINSTANCE hInstance, HWND hWnd) = 0;
	virtual void OnDestroy() = 0;
 
	virtual void BuildObject() = 0;

	virtual void Animate(float DeltaTime) = 0;
	virtual void Render(HDC hDC, HDC hMemDC, HDC hMemDC2) = 0;
};

class CGameScene : public CScene
{
private:
	CMap*				m_Map{};
	CPlayer*			m_Player{};

public:
	CGameScene() = default;
	virtual ~CGameScene();

	virtual void ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessInput(float DeltaTime);

	virtual void OnCreate(HINSTANCE hInstance, HWND hWnd);
	virtual void OnDestroy();

	virtual void BuildObject();

	virtual void Animate(float DeltaTime);
	virtual void Render(HDC hDC, HDC hMemDC, HDC hMemDC2);
};
