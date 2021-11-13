#pragma once
#include "Timer.h"
#include "Player.h"
#include "Tower.h"
#include "Monster.h"
#include "Item.h"

class CMap;

struct GameData
{
	float							m_DeltaTime{};

	CPlayer							m_Players[MAX_PLAYER]{};
	CTower							m_Tower{};
	CMonster						m_Monsters[MAX_MONSTER]{};
	CItem							m_Items[MAX_ITEM]{};
};

class CScene
{
protected:
	RECT							m_ClientRect{};

	HBITMAP							m_hBitmap{};
	HBITMAP							m_hOldBitmap{};
	
public:
	CScene() = default;
	virtual ~CScene() = default;

	virtual void ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessInput() = 0;

	virtual void OnCreate(HINSTANCE hInstance, HWND hWnd, int ID, GameData* Data) = 0;
	virtual void OnDestroy() = 0;
 
	virtual void BuildObject(int ID, GameData* Data) = 0;

	virtual void Render(HDC hDC, HDC hMemDC, HDC hMemDC2) = 0;
};

class CGameScene : public CScene
{
private:
	POINT				m_CursorPos{};

	int					m_ID{};
	CMap*				m_Map{};
	GameData*			m_GameData{};
	
public:
	CGameScene() = default;
	virtual ~CGameScene();

	virtual void ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessInput();

	virtual void OnCreate(HINSTANCE hInstance, HWND hWnd, int ID, GameData* Data);
	virtual void OnDestroy();

	virtual void BuildObject(int ID, GameData* Data);

	virtual void Render(HDC hDC, HDC hMemDC, HDC hMemDC2);
};
