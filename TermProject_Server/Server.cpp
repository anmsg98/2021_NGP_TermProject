#include "stdafx.h"
#include "Server.h"
#include "Map.h"
#include "FileManager.h"

int CServer::m_RecentID;

CServer::CServer()
{
    CFileManager::GetInstance()->LoadRectFromFile("Image/SpriteCoord.txt");

    m_GameData = new GameData{};
}

CServer::~CServer()
{
    if (m_Map)
    {
        delete m_Map;
    }

    if (m_GameData)
    {
        delete m_GameData;
    }

    closesocket(m_ListenSocket);
    WSACleanup();
}

void CServer::err_quit(const char* Msg)
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

void CServer::err_display(const char* Msg)
{
    LPVOID MsgBuffer{};

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&MsgBuffer, 0, NULL);

    cout << "[" << Msg << "] " << (char*)MsgBuffer;

    LocalFree(MsgBuffer);
}

int CServer::recvn(SOCKET Socket, char* Buffer, int Length, int Flags)
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

DWORD WINAPI CServer::AcceptClient(LPVOID Arg)
{
    // 서버 자기 자신에 대한 포인터로, 스레드 함수 내에서 멤버변수에 접근하기 위해서 이와 같이 구현하였다.
    CServer* Server{ (CServer*)Arg };
    SOCKET ClientSocket{};
    SOCKADDR_IN ClientAddress{};

    while (true)
    {
        int AddressLength{ sizeof(ClientAddress) };

        ClientSocket = accept(Server->m_ListenSocket, (SOCKADDR*)&ClientAddress, &AddressLength);

        if (ClientSocket == INVALID_SOCKET)
        {
            Server->err_display("accept()");
            break;
        }

        // CreatePlayer() 함수가 false를 반환한 경우, 최대인원이 접속한 것이므로 continue를 통해 게임시작 전까지 accept() 대기 상태로 만든다.
        if (!Server->CreatePlayer(ClientSocket, ClientAddress))
        {
            continue;
        }

        cout << "[클라이언트 접속] " << "IP : " << inet_ntoa(ClientAddress.sin_addr) << ", 포트번호 : " << ntohs(ClientAddress.sin_port) << endl;

        HANDLE ThreadHandle{ CreateThread(NULL, 0, ProcessClient, (LPVOID)Server, 0, NULL) };

        if (ThreadHandle)
        {
            CloseHandle(ThreadHandle);
        }
        else
        {
            closesocket(ClientSocket);
        }
    }

    return 0;
}

DWORD WINAPI CServer::ProcessClient(LPVOID Arg)
{
    // 서버 자기 자신에 대한 포인터로, 스레드 함수 내에서 멤버변수에 접근하기 위해서 이와 같이 구현하였다.
    CServer* Server{ (CServer*)Arg };

    // 아이디로부터 가장 최근에 접속한 클라이언트의 플레이어 데이터를 가져온다.
    CPlayer* Player{ &Server->m_GameData->m_Players[m_RecentID] };
    int ID{ Player->GetID() };

    // 최초로 클라이언트에게 초기화된 플레이어의 아이디를 보낸다.
    int ReturnValue{ send(Player->GetSocket(), (char*)&ID, sizeof(int), 0) };

    if (ReturnValue == SOCKET_ERROR)
    {
        Server->err_display("send()");
    }
    
    while (true)
    {
        ReturnValue = send(Player->GetSocket(), (char*)Server->m_GameData, sizeof(GameData), 0);

        if (ReturnValue == SOCKET_ERROR)
        {
            Server->err_display("send()");
            break;
        }

        ReturnValue = Server->recvn(Player->GetSocket(), (char*)Player, sizeof(CPlayer), 0);

        if (ReturnValue == SOCKET_ERROR)
        {
            Server->err_display("recv()");
            break;
        }
        else if (ReturnValue == 0)
        {
            break;
        }

        SetEvent(Server->m_SyncHandles[ID]);
        WaitForSingleObject(Server->m_MainSyncHandle, INFINITE);
    }

    cout << "[클라이언트 종료] " << "IP : " << inet_ntoa(Player->GetSocketAddress().sin_addr) << ", 포트번호 : " << ntohs(Player->GetSocketAddress().sin_port) << endl;

    closesocket(Player->GetSocket());

    // 접속이 종료된 클라이언트의 플레이어 객체는 값을 초기값으로 만든다.
    Server->DestroyPlayer(Player->GetID());

    return 0;
}

void CServer::InitServer()
{
    WSADATA Wsa{};

    if (WSAStartup(MAKEWORD(2, 2), &Wsa))
    {
        cout << "윈속을 초기화하지 못했습니다." << endl;
        exit(1);
    }

    m_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (m_ListenSocket == INVALID_SOCKET)
    {
        err_quit("socket()");
    }

    m_SocketAddress.sin_family = AF_INET;
    m_SocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    m_SocketAddress.sin_port = htons(SERVER_PORT);

    int ReturnValue{};

    ReturnValue = bind(m_ListenSocket, (SOCKADDR*)&m_SocketAddress, sizeof(m_SocketAddress));

    if (ReturnValue == SOCKET_ERROR)
    {
        err_quit("bind()");
    }

    ReturnValue = listen(m_ListenSocket, SOMAXCONN);

    if (ReturnValue == SOCKET_ERROR)
    {
        err_quit("listen()");
    }

    InitEvent();
    BuildObject();
    InitWaitingScene();

    HANDLE hThread{ CreateThread(NULL, 0, AcceptClient, (LPVOID)this, 0, NULL) };

    if (hThread)
    {
        CloseHandle(hThread);
    }
}

void CServer::InitEvent()
{
    m_MainSyncHandle = CreateEvent(NULL, TRUE, FALSE, NULL);

    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        m_SyncHandles[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
}

void CServer::BuildObject()
{
    // 맵을 생성하고 초기화한다.
    RECT MapRect{ 0, 0, 2400, 1500 };

    m_Map = new CMap{};
    m_Map->SetRect(MapRect);

    // 타워를 초기화한다.
    m_GameData->m_Tower.SetWidth(228.0f);
    m_GameData->m_Tower.SetHeight(213.0f);

    // 모든 플레이어를 초기화한다.
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        m_GameData->m_Players[i].SetWidth(62.5f);
        m_GameData->m_Players[i].SetHeight(90.0f);
    }

    // 모든 몬스터를 초기화한다.
    for (int i = 0; i < MAX_MONSTER; ++i)
    {
        m_GameData->m_Monsters[i].SetWidth(73.5f);
        m_GameData->m_Monsters[i].SetHeight(76.0f);
    }

    // 아이템을 초기화한다.
    for (int i = 0; i < MAX_ITEM; ++i)
    {
        m_GameData->m_Items[i].SetWidth(34.0f);
        m_GameData->m_Items[i].SetHeight(40.0f);
    }
}

void CServer::InitGameScene()
{
    // 타워를 초기화한다.
    m_GameData->m_Tower.SetActive(true);
    m_GameData->m_Tower.SetMaxHp(5000.0f);
    m_GameData->m_Tower.SetHp(5000.0f);
    m_GameData->m_Tower.SetPosition(0.5f * m_Map->GetRect().right, 0.5f * m_Map->GetRect().bottom);

    // 모든 플레이어를 초기화한다.
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetSocket())
        {
            m_GameData->m_Players[i].SetActive(true);
            m_GameData->m_Players[i].SetReady(false);
            m_GameData->m_Players[i].SetMaxHp(100.0f);
            m_GameData->m_Players[i].SetHp(100.0f);
            m_GameData->m_Players[i].SetDirection(0.0f, 0.0f);
            m_GameData->m_Players[i].SetPosition(0.5f * m_Map->GetRect().right - 250.0f + (i % 2 * 500.0f), 0.5f * m_Map->GetRect().bottom - 250.0f + (i / 2 * 500.0f));
        }
    }

    // 모든 몬스터를 초기화한다.
    for (int i = 0; i < MAX_MONSTER; ++i)
    {
        m_GameData->m_Monsters[i].SetActive(false);
    }

    // 아이템을 초기화한다.
    for (int i = 0; i < MAX_ITEM; ++i)
    {
        m_GameData->m_Items[i].SetActive(false);
    }

    // 라운드 초기화
    m_Round = 1;
    m_CurrentMonsterGenTime = 0.0f;
    m_TotalMonsterCount = 0;
    m_CurrentMonsterCount = 0;
    m_CurrentItemGenTime = 0.0f;
}

void CServer::InitWaitingScene()
{
    // 모든 플레이어를 초기화한다.
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetSocket())
        {
            m_GameData->m_Players[i].SetActive(true);
            m_GameData->m_Players[i].SetReady(false);
            m_GameData->m_Players[i].SetHp(0.0f);
            m_GameData->m_Players[i].SetDirection(0.0f, 90.0f);
            m_GameData->m_Players[i].SetPosition(0.5f * m_GameData->m_Players[i].GetWidth() + 135.0f + 157.0f * i, 0.5f * m_GameData->m_Players[i].GetHeight() + 178.0f);
        }
    }
}

void CServer::ProcessGameData()
{
    while (true)
    {
        for (int i = 0; i < MAX_PLAYER; ++i)
        {
            if (m_GameData->m_Players[i].GetSocket())
            {
                WaitForSingleObject(m_SyncHandles[i], INFINITE);
            }
        }

        cout << "송수신완료" << endl;

        switch (m_GameData->m_State)
        {
        case GAME_STATE::WAITING:
            WaitingLoop();
            break;
        case GAME_STATE::GAME:
            GameLoop();
            break;
        }
        
        cout << "루프완료" << endl;

        SetEvent(m_MainSyncHandle);
        ResetEvent(m_MainSyncHandle);
    }
}

void CServer::WaitingLoop()
{
    if (CheckAllPlayerReady())
    {
        InitGameScene();
        m_GameData->m_State = GAME;
    }
}

void CServer::GameLoop()
{
    UpdateRound();

    Animate();

    CheckPlayerByMonsterCollision();
    CheckBulletByMonsterCollision();
    CheckTowerByMonsterCollision();
    CheckPlayerByItemCollision();

    if (CheckGameOver())
    {
        InitWaitingScene();
        m_GameData->m_State = WAITING;
    }
}

int CServer::GetValidID() const
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (!m_GameData->m_Players[i].GetSocket())
        {
            // 플레이어의 소켓 값이 NULL이라면 클라이언트에게 할당된 플레이어가 아니므로 해당 아이디를 반환한다.
            return i;
        }
    }

    // 만약 반복문을 빠져 나온다면, 4명의 클라이언트가 접속한 것을 의미하므로 유효하지 않은 아이디인 -1을 반환한다.
    return -1;
}

bool CServer::CreatePlayer(SOCKET Socket, const SOCKADDR_IN& SocketAddress)
{
    int ValidID{ GetValidID() };

    // GetValidID() 함수가 -1을 반환한 경우에는 최대 인원이 접속한 경우이므로, 게임 시작전까지는 accept() 함수에서 대기하도록 한다.
    if (ValidID == -1)
    {
        return false;
    }

    m_GameData->m_Players[ValidID].SetID(ValidID);
    m_GameData->m_Players[ValidID].SetSocket(Socket);
    m_GameData->m_Players[ValidID].SetSocketAddress(SocketAddress);
    m_GameData->m_Players[ValidID].SetActive(true);
    m_GameData->m_Players[ValidID].SetReady(false);
    m_GameData->m_Players[ValidID].SetDirection(0.0f, 90.0f);
    m_GameData->m_Players[ValidID].SetPosition(0.5f * m_GameData->m_Players[ValidID].GetWidth() + 135.0f + 157.0f * ValidID, 0.5f * m_GameData->m_Players[ValidID].GetHeight() + 178.0f);

    // 가장 최근에 사용된 유효한 아이디를 갱신한다.
    m_RecentID = ValidID;
    // 현재 플레이어 수 추가
    m_PlayerCount++;

    return true;
}

bool CServer::DestroyPlayer(int ID)
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetID() == ID)
        {
            // 매개변수로 넘어온 아이디를 가진 플레이어가 있다면 소켓을 NULL로 만들어 다른 클라이언트가 접속할 수 있게 한다.
            m_GameData->m_Players[i].SetSocket(NULL);
            m_GameData->m_Players[i].SetActive(false);
            m_PlayerCount--;
            
            SetEvent(m_SyncHandles[i]);

            return true;
        }
    }

    // 만약 반복문을 빠져 나온다면, 해당 아이디를 가진 플레이어는 존재하지 않는 것이므로 false를 반환한다.
    return false;
}

bool CServer::CheckAllPlayerReady()
{
    // 플레이어가 한명 이하일 때는 시작을 하지않는다.
    if (m_PlayerCount < 2)
    {
        return false;
    }

    int ReadyCount{};

    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetSocket() && m_GameData->m_Players[i].IsReady() == true)
        {
            ReadyCount++;
        }        
    }

    if (ReadyCount == m_PlayerCount)
    {
        return true;
    }

    return false;
}

bool CServer::CheckGameOver()
{
    // 타워의 체력이 0이하가 되면 true를 반환한다.
    if (m_GameData->m_Tower.GetHp() <= 0.0f)
    {
        return true;
    }

    // 접속한 플레이어들의 체력이 0이하인지 검사하고, 한 플레이어라도 0초과인 경우 false를 반환한다.
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetSocket() && m_GameData->m_Players[i].GetHp() > 0.0f)
        {
            return false;
        }
    }

    // 모든 플레이어의 체력이 0이하라면 true를 반환한다.
    return true;
}

void CServer::Animate()
{
    m_GameData->m_Tower.Animate();

    for (int i = 0; i < MAX_MONSTER; ++i)
    {
        m_GameData->m_Monsters[i].Animate();
    }

    for (int i = 0; i < MAX_ITEM; ++i)
    {
        m_GameData->m_Items[i].Animate();
    }

    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        m_GameData->m_Players[i].Animate();
    }
}

void CServer::UpdateRound()
{
    CreateMonster();
    CreateItem();

    if (m_TotalMonsterCount == 30 && m_CurrentMonsterCount == 0)
    {
        cout << "==== " << m_Round << " ROUND CLEAR ====" << endl;

        ++m_Round;
        m_TotalMonsterCount = 0;
    }
    else
    {
        cout << "\r현재 몬스터 수 / 총 몬스터 수 : " << m_CurrentMonsterCount << " / " << m_TotalMonsterCount;
    }
}

void CServer::CreateMonster()
{
    if (m_TotalMonsterCount == 30)
    {
        return;
    }

    m_CurrentMonsterGenTime += 1.0f;

    if (m_CurrentMonsterGenTime >= m_MonsterGenTime)
    {
        // 몬스터의 생성은 10마리씩 3회 호출되며, GenCount는 1회 호출 시, 10마리로 제한을 하기 위한 변수이다.
        int GenCount{};

        for (int i = 0; i < MAX_MONSTER; ++i)
        {
            if (!m_GameData->m_Monsters[i].IsActive())
            {
                // 몬스터의 종류 설정
                int Type{ rand() % 3 + 1 };

                m_GameData->m_Monsters[i].SetActive(true);
                m_GameData->m_Monsters[i].SetType(Type);
                m_GameData->m_Monsters[i].SetMaxHp(1.0f * Type * m_Round);
                m_GameData->m_Monsters[i].SetHp(1.0f * Type * m_Round);

                Type = rand() % 4;

                switch (Type)
                {
                case MONSTER_GEN_LOCATION::TOP:
                    m_GameData->m_Monsters[i].SetPosition(RandF((float)m_Map->GetRect().left + 30.0f, (float)m_Map->GetRect().right - 30.0f), (float)m_Map->GetRect().top + 30.0f);
                    break;
                case MONSTER_GEN_LOCATION::BOTTOM:
                    m_GameData->m_Monsters[i].SetPosition(RandF((float)m_Map->GetRect().left + 30.0f, (float)m_Map->GetRect().right - 30.0f), (float)m_Map->GetRect().bottom - 30.0f);
                    break;
                case MONSTER_GEN_LOCATION::LEFT:
                    m_GameData->m_Monsters[i].SetPosition((float)m_Map->GetRect().left + 30.0f, RandF((float)m_Map->GetRect().top + 30.0f, (float)m_Map->GetRect().bottom - 30.0f));
                    break;
                case MONSTER_GEN_LOCATION::RIGHT:
                    m_GameData->m_Monsters[i].SetPosition((float)m_Map->GetRect().right - 30.0f, RandF((float)m_Map->GetRect().top + 30.0f, (float)m_Map->GetRect().bottom - 30.0f));
                    break;
                }

                // 맵의 중앙을 도착점으로 지정
                m_GameData->m_Monsters[i].SetDirection((float)m_Map->GetRect().right * 0.5f - m_GameData->m_Monsters[i].GetPosition().m_X, (float)m_Map->GetRect().bottom * 0.5f - m_GameData->m_Monsters[i].GetPosition().m_Y);
                m_GameData->m_Monsters[i].SetLength(sqrtf(powf((float)m_GameData->m_Monsters[i].GetDirection().m_X, 2) + powf((float)m_GameData->m_Monsters[i].GetDirection().m_Y, 2)));

                ++GenCount;

                if (GenCount == 10)
                {
                    break;
                }
            }
        }

        m_CurrentMonsterGenTime = 0.0f;
        m_TotalMonsterCount += GenCount;
        m_CurrentMonsterCount += GenCount;

        cout << "<<<<< WAVE " << m_TotalMonsterCount / GenCount << " START! >>>>>" << endl;
    }
}

void CServer::CreateItem()
{
    m_CurrentItemGenTime += 0.01f;

    if (m_CurrentItemGenTime >= m_ItemGenTime)
    {
        for (int i = 0; i < MAX_ITEM; ++i)
        {
            if (!m_GameData->m_Items[i].IsActive())
            {
                // 아이템의 종류 설정
                int Type{ rand() % 2 + 1 };

                m_GameData->m_Items[i].SetActive(true);
                m_GameData->m_Items[i].SetType(Type);
                m_GameData->m_Items[i].SetMaxHp(60.0f);
                m_GameData->m_Items[i].SetHp(60.0f);
                m_GameData->m_Items[i].SetPosition(RandF((float)m_Map->GetRect().left + 100.0f, (float)m_Map->GetRect().right - 100.0f), RandF((float)m_Map->GetRect().top + 100.0f, (float)m_Map->GetRect().bottom - 100.0f));

                break;
            }
        }
        m_CurrentItemGenTime = 0.0f;
    }
}

void CServer::CheckPlayerByMonsterCollision()
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        // 서버와 연결 중이고 생존 한 플레이어에 대해서만 충돌을 검사한다.
        if (m_GameData->m_Players[i].GetSocket() && m_GameData->m_Players[i].IsActive())
        {
            RECT CollidedRect{};
            RECT PlayerRect{ (int)(m_GameData->m_Players[i].GetPosition().m_X - 0.5f * m_GameData->m_Players[i].GetWidth()),
                             (int)(m_GameData->m_Players[i].GetPosition().m_Y - 0.5f * m_GameData->m_Players[i].GetHeight()),
                             (int)(m_GameData->m_Players[i].GetPosition().m_X + 0.5f * m_GameData->m_Players[i].GetWidth()),
                             (int)(m_GameData->m_Players[i].GetPosition().m_Y + 0.5f * m_GameData->m_Players[i].GetHeight()) };

            for (int j = 0; j < MAX_MONSTER; ++j)
            {
                if (m_GameData->m_Monsters[j].IsActive() && !m_GameData->m_Monsters[j].IsCollided())
                {
                    RECT CollidedRect{};
                    RECT MonsterRect{ (int)(m_GameData->m_Monsters[j].GetPosition().m_X - 0.5f * m_GameData->m_Monsters[j].GetWidth()),
                                      (int)(m_GameData->m_Monsters[j].GetPosition().m_Y - 0.5f * m_GameData->m_Monsters[j].GetHeight()),
                                      (int)(m_GameData->m_Monsters[j].GetPosition().m_X + 0.5f * m_GameData->m_Monsters[j].GetWidth()),
                                      (int)(m_GameData->m_Monsters[j].GetPosition().m_Y + 0.5f * m_GameData->m_Monsters[j].GetHeight()) };

                    if (IntersectRect(&CollidedRect, &PlayerRect, &MonsterRect))
                    {
                        m_GameData->m_Players[i].SetHp(m_GameData->m_Players[i].GetHp() - 2.5f * m_GameData->m_Monsters[i].GetType());
                        m_GameData->m_Monsters[j].PrepareCollision();
                        m_GameData->m_Monsters[j].SetPrevDirection(m_GameData->m_Monsters[j].GetDirection().m_X, m_GameData->m_Monsters[j].GetDirection().m_Y);
                        m_GameData->m_Monsters[j].SetDirection(-m_GameData->m_Monsters[j].GetDirection().m_X, -m_GameData->m_Monsters[j].GetDirection().m_Y);
                    }
                }
            }
        }
    }
}

void CServer::CheckBulletByMonsterCollision()
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        // 서버와 연결 중이고 생존 한 플레이어에 대해서만 충돌을 검사한다.
        if (m_GameData->m_Players[i].GetSocket() && m_GameData->m_Players[i].IsActive())
        {
            CBullet* Bullets{ m_GameData->m_Players[i].GetBullets() };

            for (int j = 0; j < MAX_BULLET; ++j)
            {
                if (Bullets[j].IsActive() && !Bullets[j].IsCollided())
                {
                    RECT BulletRect{ (int)(Bullets[j].GetPosition().m_X - 0.5f * Bullets[j].GetWidth()),
                                     (int)(Bullets[j].GetPosition().m_Y - 0.5f * Bullets[j].GetHeight()),
                                     (int)(Bullets[j].GetPosition().m_X + 0.5f * Bullets[j].GetWidth()),
                                     (int)(Bullets[j].GetPosition().m_Y + 0.5f * Bullets[j].GetHeight()) };

                    for (int k = 0; k < MAX_MONSTER; ++k)
                    {
                        if (m_GameData->m_Monsters[k].IsActive() && !m_GameData->m_Monsters[k].IsCollided())
                        {
                            RECT CollidedRect{};
                            RECT MonsterRect{ (int)(m_GameData->m_Monsters[k].GetPosition().m_X - 0.5f * m_GameData->m_Monsters[k].GetWidth()),
                                              (int)(m_GameData->m_Monsters[k].GetPosition().m_Y - 0.5f * m_GameData->m_Monsters[k].GetHeight()),
                                              (int)(m_GameData->m_Monsters[k].GetPosition().m_X + 0.5f * m_GameData->m_Monsters[k].GetWidth()),
                                              (int)(m_GameData->m_Monsters[k].GetPosition().m_Y + 0.5f * m_GameData->m_Monsters[k].GetHeight()) };

                            if (IntersectRect(&CollidedRect, &BulletRect, &MonsterRect))
                            {
                                Bullets[j].PrepareCollision();

                                m_GameData->m_Monsters[k].PrepareCollision();
                                m_GameData->m_Monsters[k].SetHp(m_GameData->m_Monsters[k].GetHp() - Bullets[j].GetAttackPower());
                                m_GameData->m_Monsters[k].SetPrevDirection(m_GameData->m_Monsters[k].GetDirection().m_X, m_GameData->m_Monsters[k].GetDirection().m_Y);
                                m_GameData->m_Monsters[k].SetDirection(Bullets[j].GetDirection().m_X, Bullets[j].GetDirection().m_Y);
                                m_GameData->m_Monsters[k].SetLength(Bullets[j].GetLength());

                                if (m_GameData->m_Monsters[k].GetHp() <= 0.0f)
                                {
                                    --m_CurrentMonsterCount;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void CServer::CheckTowerByMonsterCollision()
{
    if (m_GameData->m_Tower.IsActive())
    {
        RECT CollidedRect{};
        RECT TowerRect{ (int)(m_GameData->m_Tower.GetPosition().m_X - 0.5f * m_GameData->m_Tower.GetWidth()),
                        (int)(m_GameData->m_Tower.GetPosition().m_Y - 0.5f * m_GameData->m_Tower.GetHeight()),
                        (int)(m_GameData->m_Tower.GetPosition().m_X + 0.5f * m_GameData->m_Tower.GetWidth()),
                        (int)(m_GameData->m_Tower.GetPosition().m_Y + 0.5f * m_GameData->m_Tower.GetHeight()) };

        for (int i = 0; i < MAX_MONSTER; ++i)
        {
            if (m_GameData->m_Monsters[i].IsActive() && !m_GameData->m_Monsters[i].IsCollided())
            {
                RECT MonsterRect{ (int)(m_GameData->m_Monsters[i].GetPosition().m_X - 0.5f * m_GameData->m_Monsters[i].GetWidth()),
                                  (int)(m_GameData->m_Monsters[i].GetPosition().m_Y - 0.5f * m_GameData->m_Monsters[i].GetHeight()),
                                  (int)(m_GameData->m_Monsters[i].GetPosition().m_X + 0.5f * m_GameData->m_Monsters[i].GetWidth()),
                                  (int)(m_GameData->m_Monsters[i].GetPosition().m_Y + 0.5f * m_GameData->m_Monsters[i].GetHeight()) };

                if (IntersectRect(&CollidedRect, &TowerRect, &MonsterRect))
                {
                    m_GameData->m_Tower.SetHp(m_GameData->m_Tower.GetHp() - 10.0f * m_GameData->m_Monsters[i].GetType());
                    m_GameData->m_Monsters[i].PrepareCollision();
                    m_GameData->m_Monsters[i].SetPrevDirection(m_GameData->m_Monsters[i].GetDirection().m_X, m_GameData->m_Monsters[i].GetDirection().m_Y);
                    m_GameData->m_Monsters[i].SetDirection(-m_GameData->m_Monsters[i].GetDirection().m_X, -m_GameData->m_Monsters[i].GetDirection().m_Y);
                }
            }
        }
    }
}

void CServer::CheckPlayerByItemCollision()
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        // 서버와 연결 중이고 생존 한 플레이어에 대해서만 충돌을 검사한다.
        if (m_GameData->m_Players[i].GetSocket() && m_GameData->m_Players[i].IsActive())
        {
            RECT CollidedRect{};
            RECT PlayerRect{ (int)(m_GameData->m_Players[i].GetPosition().m_X - 0.5f * m_GameData->m_Players[i].GetWidth()),
                             (int)(m_GameData->m_Players[i].GetPosition().m_Y - 0.5f * m_GameData->m_Players[i].GetHeight()),
                             (int)(m_GameData->m_Players[i].GetPosition().m_X + 0.5f * m_GameData->m_Players[i].GetWidth()),
                             (int)(m_GameData->m_Players[i].GetPosition().m_Y + 0.5f * m_GameData->m_Players[i].GetHeight()) };

            for (int j = 0; j < MAX_ITEM; ++j)
            {
                if (m_GameData->m_Items[j].IsActive())
                {
                    RECT ItemRect{ (int)(m_GameData->m_Items[j].GetPosition().m_X - 0.5f * m_GameData->m_Items[j].GetWidth()),
                                   (int)(m_GameData->m_Items[j].GetPosition().m_Y - 0.5f * m_GameData->m_Items[j].GetHeight()),
                                   (int)(m_GameData->m_Items[j].GetPosition().m_X + 0.5f * m_GameData->m_Items[j].GetWidth()),
                                   (int)(m_GameData->m_Items[j].GetPosition().m_Y + 0.5f * m_GameData->m_Items[j].GetHeight()) };

                    if (IntersectRect(&CollidedRect, &PlayerRect, &ItemRect))
                    {
                        if (m_GameData->m_Items[j].GetType() == CItem::ATTACK_POWER_UP)
                        {
                            m_GameData->m_Players[i].ReinforceBullet();
                        }
                        else if (m_GameData->m_Items[j].GetType() == CItem::HP_UP)
                        {
                            m_GameData->m_Players[i].SetHp(m_GameData->m_Players[i].GetHp() + 30.0f);
                        }

                        m_GameData->m_Items[j].SetActive(false);
                    }
                }
            }
        }
    }
}
