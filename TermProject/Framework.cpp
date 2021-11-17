#include "stdafx.h"
#include "Framework.h"
#include "Scene.h"

CFramework::CFramework()
{
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
	CScene* Scene{ new CGameScene{} };

	Scene->OnCreate(hInstance, hWnd, m_ID, m_GameData);
	m_Scenes.push(Scene);
}

void CFramework::OnDestroy()
{

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
		cout << "플레이어 아이디를 수신하지 못했습니다." << endl;
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
	int ReturnValue{};
	
	ReturnValue = recvn(m_Socket, (char*)m_GameData, sizeof(GameData), 0);

	if (ReturnValue == SOCKET_ERROR)
	{
		err_display("recv()");
	}

	m_Scenes.top()->ProcessInput();
	m_Scenes.top()->Animate();

	ReturnValue = send(m_Socket, (char*)m_GameData, sizeof(GameData), 0);

	if (ReturnValue == SOCKET_ERROR)
	{
		err_display("send()");
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

void CFramework::Update()
{
	ProcessInput();
	PrepareRender();
}
