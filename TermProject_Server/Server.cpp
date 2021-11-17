#include "stdafx.h"
#include "Server.h"
#include "Map.h"
#include "FileManager.h"

int CServer::m_RecentID;

CServer::CServer()
{
    CFileManager::GetInstance()->LoadRectFromFile("Image/SpriteCoord.txt");

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
        
        // ��ü �� �浹 üũ �� ó��
        Server->CheckBulletByMonsterCollision();
        Server->CheckTowerByMonsterCollision();
        Server->CheckPlayerByItemCollision();

        printf("\r[�÷��̾� %d] : (%.02f, %.02f)", Player->GetID(), Player->GetPosition().m_X, Player->GetPosition().m_Y);
    }

    cout << "[Ŭ���̾�Ʈ ����] " << "IP : " << inet_ntoa(Player->GetSocketAddress().sin_addr) << ", ��Ʈ��ȣ : " << ntohs(Player->GetSocketAddress().sin_port) << endl;

    closesocket(Player->GetSocket());

    // ������ ����� Ŭ���̾�Ʈ�� �÷��̾� ��ü�� ���� �ʱⰪ���� �����.
    Server->DestroyPlayer(Player->GetID());

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
    m_GameData->m_Players[ValidID].SetHp(100.0f);
    m_GameData->m_Players[ValidID].SetPosition(0.5f * m_Map->GetRect().right - 50.0f + 50.0f * ValidID, 0.5f * m_Map->GetRect().bottom + 50.0f);

    // ���� �ֱٿ� ���� ��ValidID�̵� �����Ѵ�.
    m_RecentID = ValidID;

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
    m_Map->SetBitmapRect(CFileManager::GetInstance()->GetRect("Background"));

    // Ÿ���� �ʱ�ȭ�Ѵ�.
    m_GameData->m_Tower.SetActive(true);
    m_GameData->m_Tower.SetHp(5000.0f);
    m_GameData->m_Tower.SetPosition(0.5f * MapRect.right, 0.5f * MapRect.bottom);
    m_GameData->m_Tower.SetBitmapRect(CFileManager::GetInstance()->GetRect("Tower_1_1"));
    m_GameData->m_Tower.SetWidth(228.0f);
    m_GameData->m_Tower.SetHeight(213.0f);

    // ��� �÷��̾ �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_PLAYER; ++i)
    {
        m_GameData->m_Players[i].SetActive(false);
        m_GameData->m_Players[i].SetHp(100.0f);
        m_GameData->m_Players[i].SetPosition(0.5f * MapRect.right - 150.0f + 80.0f * i, 0.5f * MapRect.bottom + 100.0f);
        m_GameData->m_Players[i].SetWidth(62.5f);
        m_GameData->m_Players[i].SetHeight(90.0f);
    }

    m_GameData->m_Players[0].SetBitmapRect(CFileManager::GetInstance()->GetRect("Player_1"));
    m_GameData->m_Players[1].SetBitmapRect(CFileManager::GetInstance()->GetRect("Player_2"));
    m_GameData->m_Players[2].SetBitmapRect(CFileManager::GetInstance()->GetRect("Player_3"));
    m_GameData->m_Players[3].SetBitmapRect(CFileManager::GetInstance()->GetRect("Player_4"));

    // ��� ���͸� �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_MONSTER; ++i)
    {
         // ������ ���� ����
        int Type{ rand() % 3 + 1 };

        switch (Type)
        {
        case CMonster::LOWER:
            m_GameData->m_Monsters[i].SetBitmapRect(CFileManager::GetInstance()->GetRect("Monster_1_1"));
            break;
        case CMonster::MIDDLE:
            m_GameData->m_Monsters[i].SetBitmapRect(CFileManager::GetInstance()->GetRect("Monster_2_1"));
            break;
        case CMonster::UPPER:
            m_GameData->m_Monsters[i].SetBitmapRect(CFileManager::GetInstance()->GetRect("Monster_3_1"));
            break;
        }

        m_GameData->m_Monsters[i].SetActive(true);
        m_GameData->m_Monsters[i].SetType(Type);
        m_GameData->m_Monsters[i].SetHp(100.0f * Type);
        m_GameData->m_Monsters[i].SetWidth(73.5f);
        m_GameData->m_Monsters[i].SetHeight(76.0f);

        // ������ ���� ���� ����
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
        m_GameData->m_Monsters[i].SetDirection((float)m_Map->GetRect().right * 0.5f - m_GameData->m_Monsters[i].GetPosition().m_X, (float)m_Map->GetRect().bottom * 0.5f - m_GameData->m_Monsters[i].GetPosition().m_Y);
        m_GameData->m_Monsters[i].SetLength(sqrtf(powf((float)m_GameData->m_Monsters[i].GetDirection().m_X, 2) + powf((float)m_GameData->m_Monsters[i].GetDirection().m_Y, 2)));
    }

    // �������� �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < MAX_ITEM; ++i)
    {
        // �������� ���� ����
        int Type{ rand() % 2 + 1 };

        switch (Type)
        {
        case CItem::HP_UP:
            m_GameData->m_Items[i].SetBitmapRect(CFileManager::GetInstance()->GetRect("Potion_1"));
            break;
        case CItem::ATTACK_POWER_UP:
            m_GameData->m_Items[i].SetBitmapRect(CFileManager::GetInstance()->GetRect("Potion_2"));
            break;
        }

        m_GameData->m_Items[i].SetActive(true);
        m_GameData->m_Items[i].SetType(Type);
        m_GameData->m_Items[i].SetHp(60.0f);
        m_GameData->m_Items[i].SetPosition(0.5f * MapRect.right - 120.0f + 80.0f * i, 0.5f * MapRect.bottom - 150.0f);
        m_GameData->m_Items[i].SetWidth(34.0f);
        m_GameData->m_Items[i].SetHeight(40.0f);
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

    //            Monster->SetDirection((float)m_Map->GetRect().right * 0.5f - Monster->GetPosition().m_X, (float)m_Map->GetRect().bottom * 0.5f - Monster->GetPosition().m_Y);
    //            Monster->SetLength(sqrtf(powf((float)Monster->GetDirection().x, 2) + powf((float)Monster->GetDirection().y, 2)));

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
    //m_CurrentItemGenTime += DeltaTime;

    //if (m_CurrentItemGenTime >= m_ItemGenTime)
    //{
    //    for (const auto& Item : m_Items)
    //    {
    //        if (!Item->IsActive())
    //        {
    //            Item->SetActive(true);
    //            Item->SetHp(120);
    //            Item->SetPosition(RandF((float)m_Map->GetRect().left + 100.0f, (float)m_Map->GetRect().right - 100.0f),
    //                RandF((float)m_Map->GetRect().top + 100.0f, (float)m_Map->GetRect().bottom - 100.0f));


    //            printf("[�ȳ�] ������ ������(%.02f, %.02f)\n", Item->GetPosition().m_X, Item->GetPosition().m_Y);
    //            break;
    //        }
    //    }
    //    m_CurrentItemGenTime = 0.0f;
    //}
}

void CServer::RemoveItem()
{

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
                if (Bullets[j].IsActive())
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
                                Bullets[j].SetActive(false);

                                m_GameData->m_Monsters[k].PrepareCollision();
                                m_GameData->m_Monsters[k].SetHp(m_GameData->m_Monsters[k].GetHp() - Bullets[j].GetAttackPower());
                                m_GameData->m_Monsters[k].SetPrevDirection(m_GameData->m_Monsters[k].GetDirection().m_X, m_GameData->m_Monsters[k].GetDirection().m_Y);
                                m_GameData->m_Monsters[k].SetDirection(Bullets[j].GetDirection().m_X, Bullets[j].GetDirection().m_Y);
                                m_GameData->m_Monsters[k].SetLength(Bullets[j].GetLength());
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
                    m_GameData->m_Tower.SetHp(m_GameData->m_Tower.GetHp() - 10 * m_GameData->m_Monsters[i].GetType());
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
                            m_GameData->m_Players[i].SetHp(m_GameData->m_Players[i].GetHp() + 30);
                        }

                        m_GameData->m_Items[j].SetActive(false);
                    }
                }
            }
        }
    }
}