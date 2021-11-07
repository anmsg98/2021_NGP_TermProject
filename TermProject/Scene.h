#pragma once

class CMap;
class CPlayer;
class CMonster;

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
	POINT				m_CursorPos{};

	float               m_MonsterGenTime{ 3.0f };
	float               m_CurrentGenTime{};

	CMap*				m_Map{};
	CPlayer*			m_Player{};
	vector<CMonster*>	m_Monsters{};
	
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

	void CreateMonster(float DeltaTime);
	void CheckMonsterByBulletCollision();
};
