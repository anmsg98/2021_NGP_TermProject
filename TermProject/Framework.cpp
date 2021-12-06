#include "stdafx.h"
#include "Framework.h"
#include "Scene.h"

CFramework::CFramework()
{
	_tcscpy_s(m_Title, _T("Monkey Defense ("));

	m_GameData = new GameData{};
}

CFramework::~CFramework()
{
	if (m_GameData)
	{
		delete m_GameData;
	}

	closesocket(m_Socket);
	WSACleanup();
}

void CFramework::OnCreate(const HINSTANCE& hInstance, const HWND& hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	GetClientRect(hWnd, &m_ClientRect);
	ConnectServer();
	CFileManager::GetInstance()->LoadBitmaps(hInstance);
	CFileManager::GetInstance()->LoadRectFromFile("Image/SpriteCoord.txt");

	// 씬을 생성한다.
	CScene* Scene{ new CWaitingScene{} };

	Scene->OnCreate(hInstance, hWnd, m_ID, m_GameData);
	m_Scenes.push(Scene);

	// 사운드 관리자의 인스턴스를 생성하고 타이틀 배경음악을 재생시킨다.
	CSoundManager::GetInstance()->Init();
	CSoundManager::GetInstance()->Play(CSoundManager::WAITING_BACKGROUND_SOUND, 0.5f);
}

void CFramework::OnDestroy()
{

}

void CFramework::SetWindowTitle()
{
	_itot_s(m_GameData->m_FrameRate, m_Title + 16, 47, 10);
	_tcscat_s(m_Title, 47, TEXT(" FPS)"));
	SetWindowText(m_hWnd, m_Title);
}

void CFramework::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

void CFramework::err_quit(const char* Msg)
{
	LPVOID MsgBuffer{};

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&MsgBuffer, 0, NULL);
	MessageBox(NULL, (LPCTSTR)MsgBuffer, (LPCTSTR)Msg, MB_ICONERROR);

	LocalFree(MsgBuffer);
	exit(1);
}

void CFramework::err_display(const char* Msg)
{
	LPVOID MsgBuffer{};

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&MsgBuffer, 0, NULL);

	cout << "[" << Msg << "] " << (char*)MsgBuffer << endl;

	LocalFree(MsgBuffer);
}

int CFramework::recvn(SOCKET Socket, char* Buffer, int Length, int Flags)
{
	int Received{};
	int Left{ Length };
	char* Ptr{ Buffer };

	while (Left > 0)
	{
		Received = recv(Socket, Ptr, Left, Flags);

		if (Received == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (Received == 0)
		{
			break;
		}

		Left -= Received;
		Ptr += Received;
	}

	return (Length - Left);
}

void CFramework::ConnectServer()
{
	WSADATA Wsa{};

	if (WSAStartup(MAKEWORD(2, 2), &Wsa))
	{
		cout << "윈속을 초기화하지 못했습니다." << endl;
		exit(1);
	}

	m_Socket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_Socket == INVALID_SOCKET)
	{
		err_quit("socket()");
	}

	m_SocketAddress.sin_family = AF_INET;
	m_SocketAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
	m_SocketAddress.sin_port = htons(SERVER_PORT);

	int ReturnValue{ connect(m_Socket, (SOCKADDR*)&m_SocketAddress, sizeof(m_SocketAddress)) };

	if (ReturnValue == SOCKET_ERROR)
	{
		err_quit("connect()");
	}

	if (!GetPlayerID())
	{
		MessageBox(m_hWnd, TEXT("현재 정원이 꽉찼거나, 전투중이어서 게임에 접속할 수 없습니다."), TEXT("MONKEY DEFENSE"), MB_ICONSTOP | MB_OK);
		PostQuitMessage(0);
	}
}

bool CFramework::GetPlayerID()
{
	int ReturnValue{ recvn(m_Socket, (char*)&m_ID, sizeof(int), 0) };

	if (ReturnValue == SOCKET_ERROR)
	{
		err_display("recv()");

		return false;
	}
	else if (m_ID == -1)
	{
		return false;
	}

	return true;
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
	if (m_IsActive)
	{
		m_Scenes.top()->ProcessInput();
	}
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

void CFramework::ChangeScene(int PrevState)
{
	if (PrevState != m_GameData->m_State)
	{
		if (PrevState == WAITING && m_GameData->m_State == INGAME)
		{
			CScene* Scene{ new CGameScene{} };

			Scene->OnCreate(m_hInstance, m_hWnd, m_ID, m_GameData);
			m_Scenes.push(Scene);

			CSoundManager::GetInstance()->Stop(CSoundManager::WAITING_BACKGROUND_SOUND);
			CSoundManager::GetInstance()->Play(CSoundManager::GAME_BACKGROUND_SOUND, 0.5f);
		}
		else if (PrevState == INGAME && m_GameData->m_State == WAITING)
		{
			m_Scenes.top()->OnDestroy();
			m_Scenes.pop();

			CSoundManager::GetInstance()->Stop(CSoundManager::GAME_BACKGROUND_SOUND);
			CSoundManager::GetInstance()->Play(CSoundManager::WAITING_BACKGROUND_SOUND, 0.5f);
		}
	}
}

void CFramework::Update()
{
	int PrevState{ m_GameData->m_State };
	int ReturnValue{};

	ReturnValue = recvn(m_Socket, (char*)m_GameData, sizeof(GameData), 0);

	if (ReturnValue == SOCKET_ERROR)
	{
		err_display("recv()");
	}

	ChangeScene(PrevState);
	ProcessInput();

	ReturnValue = send(m_Socket, (char*)&m_GameData->m_Players[m_ID], sizeof(CPlayer), 0);

	if (ReturnValue == SOCKET_ERROR)
	{
		err_display("send()");
	}

	PrepareRender();
	SetWindowTitle();
}
