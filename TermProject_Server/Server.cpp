#include "stdafx.h"
#include "Server.h"
#include "Timer.h"
#include "Map.h"

CServer::CServer()
{
    m_Timer = new CTimer{};
    m_GameData = new GameData{};
}

CServer::~CServer()
{
    if (m_Timer)
    {
        delete m_Timer;
    }

    if (m_GameData)
    {
        delete m_GameData;
    }

    if (m_Map)
    {
        delete m_Map;
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
    // ���� �ڱ� �ڽſ� ���� �����ͷ�, ������ �Լ� ������ ��������� �����ϱ� ���ؼ� �̿� ���� �����Ͽ���.
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

        // CreatePlayer() �Լ��� false�� ��ȯ�� ���, �ΰ��� �����̰ų� �ִ��ο��� ������ ���̹Ƿ�, continue�� ���� ���ӽ��� ������ accept() ��� ���·� �����.
        if (!Server->CreatePlayer(ClientSocket, ClientAddress))
        {
            // ���ʷ� Ŭ���̾�Ʈ���� �ʱ�ȭ�� �÷��̾��� ���̵� ������.
            int ID{ -1 };
            int ReturnValue{ send(ClientSocket, (char*)&ID, sizeof(int), 0) };

            closesocket(ClientSocket);
            continue;
        }

        cout << "[Ŭ���̾�Ʈ ����] " << "IP : " << inet_ntoa(ClientAddress.sin_addr) << ", ��Ʈ��ȣ : " << ntohs(ClientAddress.sin_port) << endl;

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
    // ���� �ڱ� �ڽſ� ���� �����ͷ�, ������ �Լ� ������ ��������� �����ϱ� ���ؼ� �̿� ���� �����Ͽ���.
    CServer* Server{ (CServer*)Arg };

    // ���̵�κ��� ���� �ֱٿ� ������ Ŭ���̾�Ʈ�� �÷��̾� �����͸� �����´�.
    CPlayer* Player{ &Server->m_GameData->m_Players[Server->m_RecentID] };
    int ID{ Player->GetID() };

    // ���ʷ� Ŭ���̾�Ʈ���� �ʱ�ȭ�� �÷��̾��� ���̵� ������.
    int ReturnValue{ send(Player->GetSocket(), (char*)&ID, sizeof(int), 0) };

    if (ReturnValue == SOCKET_ERROR)
    {
        Server->err_display("send()");
    }
  
    while (true)
    {
        WaitForSingleObject(Server->m_MainSyncHandles[0], INFINITE);

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
        WaitForSingleObject(Server->m_MainSyncHandles[1], INFINITE);
    }

    cout << "[Ŭ���̾�Ʈ ����] " << "IP : " << inet_ntoa(Player->GetSocketAddress().sin_addr) << ", ��Ʈ��ȣ : " << ntohs(Player->GetSocketAddress().sin_port) << endl;

    closesocket(Player->GetSocket());

    // ������ ����� Ŭ���̾�Ʈ�� �÷��̾� ��ü�� ���� �ʱⰪ���� �����.
    Server->DestroyPlayer(Player->GetID());

    return 0;
}

void CServer::InitServer()
{
    WSADATA Wsa{};

    if (WSAStartup(MAKEWORD(2, 2), &Wsa))
    {
        cout << "������ �ʱ�ȭ���� ���߽��ϴ�." << endl;
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
    m_MainSyncHandles[0] = CreateEvent(NULL, TRUE, TRUE, NULL);
    m_MainSyncHandles[1] = CreateEvent(NULL, TRUE, FALSE, NULL);

    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        m_SyncHandles[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
}

void CServer::BuildObject()
{
    // ���� �����ϰ� �ʱ�ȭ�Ѵ�.
    RECT MapRect{ 0, 0, 2400, 1500 };

    m_Map = new CMap{};
    m_Map->SetRect(MapRect);

    // Ÿ���� �ʱ�ȭ�Ѵ�.
    m_GameData->m_Tower.SetWidth(228.0f);
    m_GameData->m_Tower.SetHeight(213.0f);

    // ��� �÷��̾ �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        m_GameData->m_Players[i].SetWidth(62.5f);
        m_GameData->m_Players[i].SetHeight(90.0f);
    }

    // ��� ���͸� �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_MONSTER; ++i)
    {
        m_GameData->m_Monsters[i].SetWidth(73.5f);
        m_GameData->m_Monsters[i].SetHeight(76.0f);
    }

    // �������� �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_ITEM; ++i)
    {
        m_GameData->m_Items[i].SetWidth(34.0f);
        m_GameData->m_Items[i].SetHeight(40.0f);
    }
}

void CServer::InitWaitingScene()
{
    // ��� �÷��̾ �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetSocket())
        {
            m_GameData->m_Players[i].SetActive(true);
        }
        else
        {
            m_GameData->m_Players[i].SetActive(false);
        }

        m_GameData->m_Players[i].SetReady(false);
        m_GameData->m_Players[i].SetMaxHp(100.0f);
        m_GameData->m_Players[i].SetHp(0.0f);
        m_GameData->m_Players[i].SetDirection(0.0f, 90.0f);
        m_GameData->m_Players[i].SetPosition(0.5f * m_GameData->m_Players[i].GetWidth() + 135.0f + 157.0f * i, 0.5f * m_GameData->m_Players[i].GetHeight() + 178.0f);
        m_GameData->m_Players[i].ResetKillCount();
        m_GameData->m_Players[i].ResetDamageDealt();

        CBullet* Bullets{ m_GameData->m_Players[i].GetBullets() };

        for (int j = 0; j < MAX_BULLET; ++j)
        {
            Bullets[i].SetActive(false);
        }
    }

    // ���� ���� ����
    m_GameData->m_State = WAITING;
}

void CServer::InitGameScene()
{
    // Ÿ���� �ʱ�ȭ�Ѵ�.
    m_GameData->m_Tower.SetActive(true);
    m_GameData->m_Tower.SetMaxHp(10000.0f);
    m_GameData->m_Tower.SetHp(10000.0f);
    m_GameData->m_Tower.SetPosition(0.5f * m_Map->GetRect().right, 0.5f * m_Map->GetRect().bottom);

    // ��� �÷��̾ �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetSocket())
        {
            m_GameData->m_Players[i].SetActive(true);
        }
        else
        {
            m_GameData->m_Players[i].SetActive(false);
        }

        m_GameData->m_Players[i].SetReady(false);
        m_GameData->m_Players[i].SetMaxHp(100.0f);
        m_GameData->m_Players[i].SetHp(100.0f);
        m_GameData->m_Players[i].SetDirection(0.0f, 90.0f);
        m_GameData->m_Players[i].SetPosition(0.5f * m_Map->GetRect().right - 250.0f + (i % 2 * 500.0f), 0.5f * m_Map->GetRect().bottom - 250.0f + (i / 2 * 500.0f));
        m_GameData->m_Players[i].ResetKillCount();
        m_GameData->m_Players[i].ResetDamageDealt();

        CBullet* Bullets{ m_GameData->m_Players[i].GetBullets() };

        for (int j = 0; j < MAX_BULLET; ++j)
        {
            Bullets[i].SetActive(false);
        }
    }
    
    // ��� ���͸� �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_MONSTER; ++i)
    {
        m_GameData->m_Monsters[i].SetActive(false);
    }

    // ��� �������� �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_ITEM; ++i)
    {
        m_GameData->m_Items[i].SetActive(false);
    }

    // ���带 �ʱ�ȭ�Ѵ�.
    m_GameData->m_Round = 1;
    m_GameData->m_CurrentMonsterCount = 0;
    m_GameData->m_TotalMonsterCount = 0;

    m_CurrentMonsterGenTime = 0.0f;
    m_CurrentItemGenTime = 0.0f;

    // ���� ���� ����
    m_GameData->m_State = INGAME;
}

void CServer::ProcessGameData()
{
    while (true)
    {
        m_Timer->Start();

        for (int i = 0; i < MAX_PLAYER; ++i)
        {
            if (m_GameData->m_Players[i].GetSocket())
            {
                WaitForSingleObject(m_SyncHandles[i], INFINITE);
            }
        }

        m_Timer->End();
        m_Timer->Wait(SERVER_LOCK_FPS);

        ResetEvent(m_MainSyncHandles[0]);
        SetEvent(m_MainSyncHandles[1]);

        switch (m_GameData->m_State)
        {
        case GAME_STATE::WAITING:
            WaitingLoop();
            break;
        case GAME_STATE::INGAME:
            GameLoop();
            break;
        }

        ResetEvent(m_MainSyncHandles[1]);
        SetEvent(m_MainSyncHandles[0]);
    }
}

void CServer::WaitingLoop()
{
    if (CheckAllPlayerReady())
    {
        InitGameScene();
    }
}

void CServer::GameLoop()
{
    UpdateRound();
    Animate();
    CheckCollision();
    CheckGameOver();
}

int CServer::GetValidID() const
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (!m_GameData->m_Players[i].GetSocket())
        {
            // �÷��̾��� ���� ���� NULL�̶�� Ŭ���̾�Ʈ���� �Ҵ�� �÷��̾ �ƴϹǷ� �ش� ���̵� ��ȯ�Ѵ�.
            return i;
        }
    }

    // ���� �ݺ����� ���� ���´ٸ�, 4���� Ŭ���̾�Ʈ�� ������ ���� �ǹ��ϹǷ� ��ȿ���� ���� ���̵��� -1�� ��ȯ�Ѵ�.
    return -1;
}

bool CServer::CreatePlayer(SOCKET Socket, const SOCKADDR_IN& SocketAddress)
{
    // �ΰ��� ������ ��쿡�� Ŭ���̾�Ʈ�� ������ �������� �ʴ´�.
    if (m_GameData->m_State == INGAME)
    {
        return false;
    }

    int ValidID{ GetValidID() };

    // GetValidID() �Լ��� -1�� ��ȯ�� ��쿡�� �ִ� �ο��� ������ ����̹Ƿ�, ���� ������������ accept() �Լ����� ����ϵ��� �Ѵ�.
    if (ValidID == -1)
    {
        return false;
    }

    m_GameData->m_Players[ValidID].SetID(ValidID);
    m_GameData->m_Players[ValidID].SetSocket(Socket);
    m_GameData->m_Players[ValidID].SetSocketAddress(SocketAddress);
    m_GameData->m_Players[ValidID].SetActive(true);
    m_GameData->m_Players[ValidID].SetReady(false);
    m_GameData->m_Players[ValidID].SetHp(0.0f);
    m_GameData->m_Players[ValidID].SetDirection(0.0f, 90.0f);
    m_GameData->m_Players[ValidID].SetPosition(0.5f * m_GameData->m_Players[ValidID].GetWidth() + 135.0f + 157.0f * ValidID, 0.5f * m_GameData->m_Players[ValidID].GetHeight() + 178.0f);
    m_GameData->m_Players[ValidID].ResetKillCount();
    m_GameData->m_Players[ValidID].ResetDamageDealt();

    // ���� �ֱٿ� ���� ��ȿ�� ���̵� �����Ѵ�.
    m_RecentID = ValidID;

    // Ŭ���̾�Ʈ �����ϸ� ���� �������� �÷��̾��� ���� 1������Ų��.
    ++m_PlayerCount;

    return true;
}

bool CServer::DestroyPlayer(int ID)
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetID() == ID)
        {
            // �Ű������� �Ѿ�� ���̵� ���� �÷��̾ �ִٸ� ������ NULL�� ����� �ٸ� Ŭ���̾�Ʈ�� ������ �� �ְ� �Ѵ�.
            m_GameData->m_Players[i].SetSocket(NULL);
            m_GameData->m_Players[i].SetActive(false);

            // Ŭ���̾�Ʈ ����Ǹ� ���� �������� �÷��̾��� ���� 1���ҽ�Ų��.
            --m_PlayerCount;
            
            // �ش� �ڵ������ ���� ����ȭ�� �߻����� �ʵ��� ��ȣ���·� �����Ѵ�.
            SetEvent(m_SyncHandles[i]);

            return true;
        }
    }

    // ���� �ݺ����� ���� ���´ٸ�, �ش� ���̵� ���� �÷��̾�� �������� �ʴ� ���̹Ƿ� false�� ��ȯ�Ѵ�.
    return false;
}

bool CServer::CheckAllPlayerReady()
{
    // �÷��̾ ���� ��쿡�� �������� �ʴ´�.
    if (m_PlayerCount <= 0)
    {
        return false;
    }

    int ReadyCount{};

    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetSocket() && m_GameData->m_Players[i].IsReady())
        {
            ++ReadyCount;
        }        
    }

    if (ReadyCount == m_PlayerCount)
    {
        return true;
    }

    return false;
}

void CServer::CheckGameOver()
{
    // ������ �÷��̾ ���� ��쿡, ������ ���¸� ���Ƿ� ��ȯ�Ѵ�.
    if (m_PlayerCount == 0)
    {
        m_GameData->m_GameOver = false;
        m_CurrentResultTime = 0.0f;
        InitWaitingScene();
        return;
    }

    if (m_GameData->m_GameOver)
    {
        m_CurrentResultTime += 1.0f;

        if (m_CurrentResultTime >= m_ResultTime)
        {
            m_GameData->m_GameOver = false;
            m_CurrentResultTime = 0.0f;
            InitWaitingScene();
        }
    }
    else
    {
        // Ÿ���� ü���� 0���ϰ� �Ǹ� ���ӿ��� ���·� ��ȯ�Ѵ�.
        if (m_GameData->m_Tower.GetHp() <= 0.0f)
        {
            m_GameData->m_GameOver = true;
            return;
        }

        bool IsAllPlayerDead{ true };

        // ������ �÷��̾���� ü���� 0�������� �˻��ϰ�, �� �÷��̾�� 0�ʰ��� ��� ���ӿ��� ���°� ���� �ʵ��� �����.
        for (int i = 0; i < MAX_PLAYER; ++i)
        {
            if (m_GameData->m_Players[i].GetSocket() && m_GameData->m_Players[i].GetHp() > 0.0f)
            {
                IsAllPlayerDead = false;
            }
        }

        if (IsAllPlayerDead)
        {
            m_GameData->m_GameOver = true;
        }
    }
}

void CServer::UpdateRound()
{
    CreateMonster();
    SetMonstersTarget();
    CreateItem();

    if (m_GameData->m_TotalMonsterCount == MAX_MONSTER && m_GameData->m_CurrentMonsterCount == 0)
    {
        ++m_GameData->m_Round;
        m_GameData->m_TotalMonsterCount = 0;

#ifdef DEBUG_PRINT_ROUND
        cout << "==== " << m_Round << " ROUND CLEAR ====" << endl;
#endif
    }
    else
    {
#ifdef DEBUG_PRINT_ROUND
        cout << "\r���� ���� �� / �� ���� �� : " << m_CurrentMonsterCount << " / " << m_TotalMonsterCount;
#endif
    }
}

void CServer::CreateMonster()
{
    if (m_GameData->m_TotalMonsterCount == MAX_MONSTER)
    {
        return;
    }

    m_CurrentMonsterGenTime += 1.0f;

    if (m_CurrentMonsterGenTime >= m_MonsterGenTime)
    {
        // ������ ������ 10������ 3ȸ ȣ��Ǹ�, GenCount�� 1ȸ ȣ�� ��, 10������ ������ �ϱ� ���� �����̴�.
        int GenCount{};

        for (int i = 0; i < MAX_MONSTER; ++i)
        {
            if (!m_GameData->m_Monsters[i].IsActive())
            {
                // ������ ���� ����
                int Type{ rand() % 3 + 1 };

                m_GameData->m_Monsters[i].SetActive(true);
                m_GameData->m_Monsters[i].SetType(Type);
                m_GameData->m_Monsters[i].SetMaxHp(5.0f * Type * m_GameData->m_Round);
                m_GameData->m_Monsters[i].SetHp(5.0f * Type * m_GameData->m_Round);

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

                // ���� �߾��� ���������� ����
                VECTOR2D TowerPosition{ m_GameData->m_Tower.GetPosition() };

                m_GameData->m_Monsters[i].SetDirection(TowerPosition - m_GameData->m_Monsters[i].GetPosition());
                m_GameData->m_Monsters[i].SetLength(Vector::Length(m_GameData->m_Monsters[i].GetDirection()));

#ifdef DEBUG_PRINT_MONSTER_GEN_POS
                cout << "[���� ����] " << "(" << m_GameData->m_Monsters[i].GetPosition().m_X << ", " << m_GameData->m_Monsters[i].GetPosition().m_Y << ")" << endl;
#endif

                ++GenCount;

                if (GenCount == 10)
                {
                    break;
                }
            }
        }

        m_GameData->m_TotalMonsterCount += GenCount;
        m_GameData->m_CurrentMonsterCount += GenCount;
        m_CurrentMonsterGenTime = 0.0f;

#ifdef DEBUG_PRINT_WAVE
        cout << "===== WAVE " << m_TotalMonsterCount / GenCount << " START! =====" << endl;
#endif
    }

    m_GameData->m_ScheduledGenTime = (int)((m_MonsterGenTime - m_CurrentMonsterGenTime) / 60.0f);
}

void CServer::CreateItem()
{
    m_CurrentItemGenTime += 1.0f;

    if (m_CurrentItemGenTime >= m_ItemGenTime)
    {
        for (int i = 0; i < MAX_ITEM; ++i)
        {
            if (!m_GameData->m_Items[i].IsActive())
            {
                // �������� ���� ����
                int Type{ rand() % 2 + 1 };

                m_GameData->m_Items[i].SetActive(true);
                m_GameData->m_Items[i].SetType(Type);
                m_GameData->m_Items[i].SetMaxHp(900.0f);
                m_GameData->m_Items[i].SetHp(900.0f);
                m_GameData->m_Items[i].SetPosition(RandF((float)m_Map->GetRect().left + 100.0f, (float)m_Map->GetRect().right - 100.0f), RandF((float)m_Map->GetRect().top + 100.0f, (float)m_Map->GetRect().bottom - 100.0f));

#ifdef DEBUG_PRINT_ITEM_GEN_POS
                cout << "[������ ����] " << "(" << m_GameData->m_Items[i].GetPosition().m_X << ", " << m_GameData->m_Items[i].GetPosition().m_Y << ")" << endl;
#endif
                break;
            }
        }

        m_CurrentItemGenTime = 0.0f;
    }
}

void CServer::SetMonstersTarget()
{
    for (int i = 0; i < MAX_MONSTER; ++i)
    {
        // ������ �ְ�, �浹���� ���� ���Ϳ� ���ؼ��� �˻��Ѵ�.
        if (m_GameData->m_Monsters[i].IsActive() && !m_GameData->m_Monsters[i].IsCollided())
        {
            VECTOR2D MonsterPosition{ m_GameData->m_Monsters[i].GetPosition() };
            bool IsTargeted{};

            for (int j = 0; j < MAX_PLAYER; ++j)
            {
                // ������ ���� ���̰� ������ �÷��̾ ���ؼ��� �˻��Ѵ�.
                if (m_GameData->m_Players[j].GetSocket() && m_GameData->m_Players[j].IsActive())
                {
                    VECTOR2D PlayerPosition{ m_GameData->m_Players[j].GetPosition() };
                    float Dist{ Vector::Distance(MonsterPosition, PlayerPosition) };

                    // �� �������� �Ÿ��� 200.0f���ϸ� �÷��̾ ���ϵ��� �����Ѵ�.
                    if (Dist <= 200.0f)
                    {
                        m_GameData->m_Monsters[i].SetDirection(PlayerPosition - MonsterPosition);
                        m_GameData->m_Monsters[i].SetLength(Vector::Length(m_GameData->m_Monsters[i].GetDirection()));

                        IsTargeted = true;
                    }
                }
            }

            // ������ �缳������ ���ߴٸ�, Ÿ���� ���ϵ��� �����Ѵ�.
            if (!IsTargeted)
            {
                VECTOR2D TowerPosition{ m_GameData->m_Tower.GetPosition() };

                m_GameData->m_Monsters[i].SetDirection(TowerPosition - MonsterPosition);
                m_GameData->m_Monsters[i].SetLength(Vector::Length(m_GameData->m_Monsters[i].GetDirection()));
            }
        }
    }
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

void CServer::CheckCollision()
{
    CheckPlayerByMonsterCollision();
    CheckBulletByMonsterCollision();
    CheckTowerByMonsterCollision();
    CheckPlayerByItemCollision();
}

void CServer::CheckPlayerByMonsterCollision()
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        // ������ ���� ���̰� ���� �� �÷��̾ ���ؼ��� �浹�� �˻��Ѵ�.
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
                        m_GameData->m_Monsters[j].SetCollision(true);
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
        // ������ ���� ���̰� ���� �� �÷��̾ ���ؼ��� �浹�� �˻��Ѵ�.
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
                                if (m_GameData->m_Monsters[k].GetHp() < Bullets[j].GetAttackPower())
                                {
                                    m_GameData->m_Players[i].IncreaseDamageDealt((int)m_GameData->m_Monsters[k].GetHp());
                                }
                                else
                                {
                                    m_GameData->m_Players[i].IncreaseDamageDealt((int)Bullets[j].GetAttackPower());
                                }

                                Bullets[j].PrepareCollision();

                                m_GameData->m_Monsters[k].SetCollision(true);
                                m_GameData->m_Monsters[k].SetDirection(Bullets[j].GetDirection().m_X, Bullets[j].GetDirection().m_Y);
                                m_GameData->m_Monsters[k].SetLength(Bullets[j].GetLength());
                                m_GameData->m_Monsters[k].SetHp(m_GameData->m_Monsters[k].GetHp() - Bullets[j].GetAttackPower());

                                if (m_GameData->m_Monsters[k].GetHp() <= 0.0f)
                                {
                                    m_GameData->m_Players[i].IncreaseKillCount();
                                    m_GameData->m_Monsters[k].SetSound(true);
                                    --m_GameData->m_CurrentMonsterCount;
                                }
                                break;
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
                    m_GameData->m_Monsters[i].SetCollision(true);
                    m_GameData->m_Monsters[i].SetDirection(Vector::Inverse(m_GameData->m_Monsters[i].GetDirection()));
                }
            }
        }
    }
}

void CServer::CheckPlayerByItemCollision()
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        // ������ ���� ���̰� ���� �� �÷��̾ ���ؼ��� �浹�� �˻��Ѵ�.
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
                        m_GameData->m_Items[j].SetSound(true);
                        break;
                    }
                }
            }
        }
    }
}
