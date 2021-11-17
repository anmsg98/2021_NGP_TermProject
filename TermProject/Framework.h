#pragma once
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000

struct GameData;
class CScene;

class CFramework
{
private:
	bool						m_IsActive{};

	RECT						m_ClientRect{};

	HINSTANCE					m_hInstance{};
	HWND						m_hWnd{};
	PAINTSTRUCT					m_PaintStruct{};

	int							m_ID{};
	SOCKET						m_Socket{};
	SOCKADDR_IN					m_SocketAddress{};

	HDC							m_hDC{};
	HDC							m_hMemDC{};
	HDC							m_hMemDC2{};

	GameData*					m_GameData{};

	stack<CScene*>				m_Scenes{};

public:
	CFramework();
	~CFramework();

	void OnCreate(const HINSTANCE& hInstance, const HWND& hWnd);
	void OnDestroy();

	void SetActive(bool IsActive);

	// 소켓 관련 오류함수
	void err_quit(const char* Msg);
	void err_display(const char* Msg);

	// Length 바이트를 확실하게 수신
	int recvn(SOCKET Socket, char* Buffer, int Length, int Flags);

	void ConnectServer();
	bool GetPlayerID();

	void ProcessKeyboardMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ProcessMouseMessage(const HWND& hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	void ProcessInput();
	void PrepareRender();
	void Render();

	void Update();
};
