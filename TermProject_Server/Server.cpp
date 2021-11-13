#include "stdafx.h"
#include "Server.h"
#include "Map.h"

int CServer::m_RecentID;

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

DWORD WINAPI CServer::ProcessClient(LPVOID Arg)
{
    // ���� �ڱ� �ڽſ� ���� �����ͷ�, ������ �Լ� ������ ��������� �����ϱ� ���ؼ� �̿� ���� �����Ͽ���.
    CServer* Server{ (CServer*)Arg };

    // ���̵�κ��� ���� �ֱٿ� ������ Ŭ���̾�Ʈ�� �÷��̾� �����͸� �����´�.
    CPlayer* Player{ Server->GetPlayerFromID(m_RecentID) };
    int ID{ Player->GetID() };

    // ���ʷ� Ŭ���̾�Ʈ���� �ʱ�ȭ�� �÷��̾��� ���̵� ������.
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

        ReturnValue = Server->recvn(Player->GetSocket(), (char*)Server->m_GameData, sizeof(GameData), 0);

        if (ReturnValue == SOCKET_ERROR)
        {
            Server->err_display("recv()");
            break;
        }
        else if (ReturnValue == 0)
        {
            break;
        }

        Server->m_Timer->Update();
        Server->m_GameData->m_DeltaTime = Server->m_Timer->GetDeltaTime();

        printf("[�÷��̾� %d] : (%.03f, %.03f)\n", Player->GetID(), Player->GetPosition().m_X, Player->GetPosition().m_Y);
    }

    closesocket(Player->GetSocket());

    // ������ ����� Ŭ���̾�Ʈ�� �÷��̾� ���̵� ���� �����Ѵ�.
    if (Server->DestroyPlayer(Player->GetID()))
    {
        cout << "[Ŭ���̾�Ʈ ����] " << "IP : " << inet_ntoa(Player->GetSocketAddress().sin_addr) << ", ��Ʈ��ȣ : " << ntohs(Player->GetSocketAddress().sin_port) << endl;
    }

    return 0;
}

void CServer::err_quit(const char* Msg)
{
    LPVOID MsgBuffer{};

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&MsgBuffer, 0, NULL);
    MessageBox(NULL, (LPCWSTR)MsgBuffer, (LPCWSTR)Msg, MB_ICONERROR);

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

    cout << "[" << Msg << "] " << (char*)MsgBuffer << endl;

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

    BuildObject();
    m_Timer->Start();
}

void CServer::InitEvent()
{

}

void CServer::AcceptClient()
{
    SOCKET ClientSocket{};
    SOCKADDR_IN ClientAddress{};

    while (true)
    {
        int AddressLength{ sizeof(ClientAddress) };

        ClientSocket = accept(m_ListenSocket, (SOCKADDR*)&ClientAddress, &AddressLength);

        if (ClientSocket == INVALID_SOCKET)
        {
            err_display("accept()");
            break;
        }
        
        // CreatePlayer() �Լ��� false�� ��ȯ�� ���, �ִ��ο��� ������ ���̹Ƿ� continue�� ���� ���ӽ��� ������ accept() ��� ���·� �����.
        if (!CreatePlayer(ClientSocket, ClientAddress))
        {
            continue;
        }

        cout << "[Ŭ���̾�Ʈ ����] " << "IP : " << inet_ntoa(ClientAddress.sin_addr) << ", ��Ʈ��ȣ : " << ntohs(ClientAddress.sin_port) << endl;

        HANDLE ThreadHandle{ CreateThread(NULL, 0, ProcessClient, (LPVOID)this, 0, NULL) };

        if (ThreadHandle)
        {
            CloseHandle(ThreadHandle);
        }
        else
        {
            closesocket(ClientSocket);
        }
    }
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

CPlayer* CServer::GetPlayerFromID(int ID)
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetID() == ID)
        {
            // �Ű������� �Ѿ�� ���̵� ���� �÷��̾ �ִٸ� �ش� �÷��̾��� �ּ� ���� ��ȯ�Ѵ�.
            return &m_GameData->m_Players[i];
        }
    }

    // ���� �ݺ����� ���� ���´ٸ�, �ش� ���̵� ���� �÷��̾�� �������� �ʴ� ���̹Ƿ� nullptr�� ��ȯ�Ѵ�.
    return nullptr;
}

bool CServer::CreatePlayer(SOCKET Socket, const SOCKADDR_IN& SocketAddress)
{
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

    // ���� �ֱٿ� ���� ���̵� �����Ѵ�.
    m_RecentID = ValidID;

    return true;
}

bool CServer::DestroyPlayer(int ID)
{
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        if (m_GameData->m_Players[i].GetID() == ID)
        {
            // �Ű������� �Ѿ�� ���̵� ���� �÷��̾ �ִٸ� �ش� �÷��̾ �ʱ�ȭ�Ѵ�.
            ZeroMemory(&m_GameData->m_Players[i], sizeof(CPlayer));

            return true;
        }
    }

    // ���� �ݺ����� ���� ���´ٸ�, �ش� ���̵� ���� �÷��̾�� �������� �ʴ� ���̹Ƿ� false�� ��ȯ�Ѵ�.
    return false;
}

bool CServer::CheckAllPlayerReady()
{
    return false;
}

bool CServer::CheckGameOver()
{
    return false;
}

void CServer::LoopWaitingScene()
{

}

void CServer::LoopGameScene()
{

}

void CServer::BuildObject()
{
    // ���� �ʱ�ȭ�Ѵ�.
    RECT MapRect{ 0, 0, 2400, 1500 };

    m_Map = new CMap{};
    m_Map->SetRect(MapRect);

    // Ÿ���� �ʱ�ȭ�Ѵ�.
    m_GameData->m_Tower.SetActive(true);
    m_GameData->m_Tower.SetHp(5000);
    m_GameData->m_Tower.SetPosition(1200.0f, 740.0f);
    m_GameData->m_Tower.SetWidth(228.0f);
    m_GameData->m_Tower.SetHeight(213.0f);

    // ��� �÷��̾ �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        m_GameData->m_Players[i].SetActive(false);
        m_GameData->m_Players[i].SetHp(100);
        m_GameData->m_Players[i].SetPosition(50.0f + 50.0f * i, 50.0f);
        m_GameData->m_Players[i].SetWidth(62.5f);
        m_GameData->m_Players[i].SetHeight(90.0f);
    }

    // ��� ���͸� �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_MONSTER; ++i)
    {
        m_GameData->m_Monsters[i].SetActive(false);
        m_GameData->m_Monsters[i].SetWidth(73.5f);
        m_GameData->m_Monsters[i].SetHeight(76.0f);

         // ������ ���� ����
        int Type{ rand() % 3 + 1 };

        m_GameData->m_Monsters[i].SetType(Type);
        m_GameData->m_Monsters[i].SetHp(10 * Type);

        // ������ ���� ���� ����
        Type = rand() % 4;

        switch (Type)
        {
        case 0:
            m_GameData->m_Monsters[i].SetPosition((float)m_Map->GetRect().left + 30.0f, RandF((float)m_Map->GetRect().top + 30.0f, (float)m_Map->GetRect().bottom - 30.0f));
            break;
        case 1:
            m_GameData->m_Monsters[i].SetPosition((float)m_Map->GetRect().right - 30.0f, RandF((float)m_Map->GetRect().top + 30.0f, (float)m_Map->GetRect().bottom - 30.0f));
            break;
        case 2:
            m_GameData->m_Monsters[i].SetPosition(RandF((float)m_Map->GetRect().left + 30.0f, (float)m_Map->GetRect().right - 30.0f), (float)m_Map->GetRect().top + 30.0f);
            break;
        case 3:
            m_GameData->m_Monsters[i].SetPosition(RandF((float)m_Map->GetRect().left + 30.0f, (float)m_Map->GetRect().right - 30.0f), (float)m_Map->GetRect().bottom - 30.0f);
            break;
        }

        // ���� �߾��� ���������� ����
        m_GameData->m_Monsters[i].SetDirect((float)m_Map->GetRect().right * 0.5f - m_GameData->m_Monsters[i].GetPosition().m_X, (float)m_Map->GetRect().bottom * 0.5f - m_GameData->m_Monsters[i].GetPosition().m_Y);
        m_GameData->m_Monsters[i].SetLength(sqrtf(powf((float)m_GameData->m_Monsters[i].GetDirect().x, 2) + powf((float)m_GameData->m_Monsters[i].GetDirect().y, 2)));
    }

    // �������� �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_ITEM; ++i)
    {
        m_GameData->m_Items[i].SetActive(false);
        m_GameData->m_Items[i].SetWidth(34.0f);
        m_GameData->m_Items[i].SetHeight(40.0f);

        // �������� ���� ����
        int Type{ rand() % 2 + 1 };

        m_GameData->m_Items[i].SetType(Type);
        m_GameData->m_Items[i].SetHp(120);
    }
}

void CServer::CreateMonster()
{
    //m_CurrentMonsterGenTime += DeltaTime;

    //if (m_CurrentMonsterGenTime >= m_MonsterGenTime)
    //{
    //    for (const auto& Monster : m_Monsters)
    //    {
    //        if (!Monster->IsActive())
    //        {
    //            Monster->SetActive(true);
    //            Monster->SetHp(10 * Monster->GetType());

    //            switch (Monster->GetType())
    //            {
    //            case 0:
    //                Monster->SetPosition((float)m_Map->GetRect().left + 30.0f, RandF((float)m_Map->GetRect().top + 30.0f, (float)m_Map->GetRect().bottom - 30.0f));
    //                break;
    //            case 1:
    //                Monster->SetPosition((float)m_Map->GetRect().right - 30.0f, RandF((float)m_Map->GetRect().top + 30.0f, (float)m_Map->GetRect().bottom - 30.0f));
    //                break;
    //            case 2:
    //                Monster->SetPosition(RandF((float)m_Map->GetRect().left + 30.0f, (float)m_Map->GetRect().right - 30.0f), (float)m_Map->GetRect().top + 30.0f);
    //                break;
    //            case 3:
    //                Monster->SetPosition(RandF((float)m_Map->GetRect().left + 30.0f, (float)m_Map->GetRect().right - 30.0f), (float)m_Map->GetRect().bottom - 30.0f);
    //            }

    //            Monster->SetDirect((float)m_Map->GetRect().right * 0.5f - Monster->GetPosition().m_X, (float)m_Map->GetRect().bottom * 0.5f - Monster->GetPosition().m_Y);
    //            Monster->SetLength(sqrtf(powf((float)Monster->GetDirect().x, 2) + powf((float)Monster->GetDirect().y, 2)));

    //            printf("[�ȳ�] ���� ������(%.02f, %.02f)\n", Monster->GetPosition().m_X, Monster->GetPosition().m_Y);
    //            break;
    //        }
    //    }
    //    m_CurrentMonsterGenTime = 0.0f;
    //}
}

void CServer::RemoveMonster()
{

}

void CServer::CreateItem()
{
 /*   m_CurrentItemGenTime += DeltaTime;

    if (m_CurrentItemGenTime >= m_ItemGenTime)
    {
        for (const auto& Item : m_Items)
        {
            if (!Item->IsActive())
            {
                Item->SetActive(true);
                Item->SetHp(120);
                Item->SetPosition(RandF((float)m_Map->GetRect().left + 100.0f, (float)m_Map->GetRect().right - 100.0f),
                    RandF((float)m_Map->GetRect().top + 100.0f, (float)m_Map->GetRect().bottom - 100.0f));


                printf("[�ȳ�] ������ ������(%.02f, %.02f)\n", Item->GetPosition().m_X, Item->GetPosition().m_Y);
                break;
            }
        }
        m_CurrentItemGenTime = 0.0f;
    }*/
}

void CServer::RemoveItem()
{

}

void CServer::CheckPlayerByItemCollision()
{
    //if (m_Player->IsActive())
    //{
    //    RECT CollidedRect{};
    //    RECT PlayerRect{ (int)(m_Player->GetPosition().m_X - 0.5f * m_Player->GetWidth()),
    //                     (int)(m_Player->GetPosition().m_Y - 0.5f * m_Player->GetHeight()),
    //                     (int)(m_Player->GetPosition().m_X + 0.5f * m_Player->GetWidth()),
    //                     (int)(m_Player->GetPosition().m_Y + 0.5f * m_Player->GetHeight()) };

    //    for (const auto& Item : m_Items)
    //    {
    //        if (Item->IsActive())
    //        {
    //            RECT ItemRect{ (int)(Item->GetPosition().m_X - 0.5f * Item->GetWidth()),
    //                           (int)(Item->GetPosition().m_Y - 0.5f * Item->GetHeight()),
    //                           (int)(Item->GetPosition().m_X + 0.5f * Item->GetWidth()),
    //                           (int)(Item->GetPosition().m_Y + 0.5f * Item->GetHeight()) };

    //            if (IntersectRect(&CollidedRect, &PlayerRect, &ItemRect))
    //            {
    //                if (Item->GetType() == CItem::ATTACK_POWER_UP)
    //                {
    //                    m_Player->ReinforceBullet();
    //                }
    //                else if (Item->GetType() == CItem::HP_UP)
    //                {
    //                    m_Player->SetHp(m_Player->GetHp() + 30);
    //                }

    //                Item->SetActive(false);
    //            }
    //        }
    //    }
    //}
}

void CServer::CheckTowerByMonsterCollision()
{
    //if (m_Tower->IsActive())
    //{
    //    RECT CollidedRect{};
    //    RECT TowerRect{ (int)(m_Tower->GetPosition().m_X - 0.5f * m_Tower->GetWidth()),
    //                    (int)(m_Tower->GetPosition().m_Y - 0.5f * m_Tower->GetHeight()),
    //                    (int)(m_Tower->GetPosition().m_X + 0.5f * m_Tower->GetWidth()),
    //                    (int)(m_Tower->GetPosition().m_Y + 0.5f * m_Tower->GetHeight()) };

    //    for (const auto& Monster : m_Monsters)
    //    {
    //        if (Monster->IsActive())
    //        {
    //            RECT MonsterRect{ (int)(Monster->GetPosition().m_X - 0.5f * Monster->GetWidth()),
    //                              (int)(Monster->GetPosition().m_Y - 0.5f * Monster->GetHeight()),
    //                              (int)(Monster->GetPosition().m_X + 0.5f * Monster->GetWidth()),
    //                              (int)(Monster->GetPosition().m_Y + 0.5f * Monster->GetHeight()) };

    //            if (IntersectRect(&CollidedRect, &TowerRect, &MonsterRect))
    //            {
    //                m_Tower->SetHp(m_Tower->GetHp() - 10 * Monster->GetType());
    //            }
    //        }
    //    }
    //}
}

void CServer::CheckBulletByMonsterCollision()
{
    //CBullet* Bullets{ m_Player->GetBullets() };
    //RECT CollidedRect{};

    //for (int i = 0; i < MAX_BULLET; ++i)
    //{
    //    if (Bullets[i].IsActive())
    //    {
    //        RECT BulletRect{ (int)(Bullets[i].GetPosition().m_X - 0.5f * Bullets[i].GetWidth()),
    //                         (int)(Bullets[i].GetPosition().m_Y - 0.5f * Bullets[i].GetHeight()),
    //                         (int)(Bullets[i].GetPosition().m_X + 0.5f * Bullets[i].GetWidth()),
    //                         (int)(Bullets[i].GetPosition().m_Y + 0.5f * Bullets[i].GetHeight()) };

    //        for (int j = 0; j < m_Monsters.size(); ++j)
    //        {
    //            if (m_Monsters[j]->IsActive())
    //            {
    //                RECT MonsterRect{ (int)(m_Monsters[j]->GetPosition().m_X - 0.5f * m_Monsters[j]->GetWidth()),
    //                                  (int)(m_Monsters[j]->GetPosition().m_Y - 0.5f * m_Monsters[j]->GetHeight()),
    //                                  (int)(m_Monsters[j]->GetPosition().m_X + 0.5f * m_Monsters[j]->GetWidth()),
    //                                  (int)(m_Monsters[j]->GetPosition().m_Y + 0.5f * m_Monsters[j]->GetHeight()) };

    //                if (IntersectRect(&CollidedRect, &BulletRect, &MonsterRect))
    //                {
    //                    Bullets[i].SetActive(false);
    //                    m_Monsters[j]->SetHp(m_Monsters[j]->GetHp() - Bullets[i].GetAttackPower());
    //                }
    //            }
    //        }
    //    }
    //}
}