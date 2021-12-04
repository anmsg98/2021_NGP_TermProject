#pragma once
#include "Player.h"
#include "Tower.h"
#include "Monster.h"
#include "Item.h"

class CMap;
class CButton;

enum GAME_STATE { WAITING, INGAME };

struct GameData
{
	int                   m_State{ WAITING };
	bool                  m_GameOver{};

	int                   m_Round{ 1 };
	int                   m_ScheduledGenTime{};          // 몬스터의 생성까지 남은시간
	int                   m_TotalMonsterCount{};         // 총 몬스터의 수
	int                   m_CurrentMonsterCount{};       // 현재 필드위에 존재하는 몬스터의 수

	CPlayer               m_Players[MAX_PLAYER]{};
	CTower                m_Tower{};
	CMonster              m_Monsters[MAX_MONSTER]{};
	CItem                 m_Items[MAX_ITEM]{};
};

class CScene
{
protected:
	POINT				m_CursorPos{};
	RECT				m_ClientRect{};

	HBITMAP				m_hBitmap{};
	HBITMAP				m_hOldBitmap{};

	HFONT				m_hFont{};
	HFONT				m_hOldFont{};

public:
	CScene() = default;
	virtual ~CScene() = default;

	virtual void ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessInput() = 0;

	virtual void OnCreate(HINSTANCE hInstance, HWND hWnd, int ID, GameData* Data) = 0;
	virtual void OnDestroy() = 0;

	virtual void BuildObject(int ID, GameData* Data) = 0;
	
	virtual void DrawSceneText(HDC hMemDC) = 0;
	virtual void Render(HDC hDC, HDC hMemDC, HDC hMemDC2) = 0;
};

// ============================================== CWaitingScene ==============================================

class CWaitingScene : public CScene
{
private:
	int						m_ID{};
	CMap*					m_Map{};
	GameData*				m_GameData{};
	CButton*				m_Buttons[MAX_BUTTON]{};

public:
	CWaitingScene() = default;
	virtual ~CWaitingScene();

	virtual void ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessInput();

	virtual void OnCreate(HINSTANCE hInstance, HWND hWnd, int ID, GameData* Data);
	virtual void OnDestroy();

	virtual void BuildObject(int ID, GameData* Data);

	virtual void DrawSceneText(HDC hMemDC);
	virtual void Render(HDC hDC, HDC hMemDC, HDC hMemDC2);
};

// ============================================== CGameScene ==============================================

class CGameScene : public CScene
{
private:
	int						m_ID{};
	CMap*					m_Map{};
	GameData*				m_GameData{};

public:
	CGameScene() = default;
	virtual ~CGameScene();

	virtual void ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessInput();

	virtual void OnCreate(HINSTANCE hInstance, HWND hWnd, int ID, GameData* Data);
	virtual void OnDestroy();

	virtual void BuildObject(int ID, GameData* Data);

	virtual void DrawSceneText(HDC hMemDC);
	virtual void Render(HDC hDC, HDC hMemDC, HDC hMemDC2);

	void DrawResultUI(HDC hMemDC, HDC hMemDC2);
};
