#pragma once

class CMap;
class CTower;
class CPlayer;
class CMonster;
class CItem;

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

	const float         m_MonsterGenTime{ 2.0f };
	const float			m_ItemGenTime{ 5.0f };

	float               m_CurrentMonsterGenTime{};
	float				m_CurrentItemGenTime{};

	CMap*				m_Map{};
	CTower*				m_Tower{};
	CPlayer*			m_Player{};
	vector<CMonster*>	m_Monsters{};
	vector<CItem*>		m_Items{};
	
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
	void CreateItem(float DeltaTime);

	void CheckPlayerByItemCollision();
	void CheckTowerByMonsterCollision();
	void CheckBulletByMonsterCollision();
};
