#pragma once
#include "Player.h"
#include "Tower.h"
#include "Monster.h"
#include "Item.h"

#define SERVER_PORT            9000
#define SERVER_LOCK_FPS       60.0f

class CMap;

enum GAME_STATE { WAITING, GAME };
enum MONSTER_GEN_LOCATION { TOP, BOTTOM, LEFT, RIGHT };

struct GameData
{
    int                   m_State{ WAITING };

    CPlayer               m_Players[MAX_PLAYER]{};
    CTower                m_Tower{};
    CMonster              m_Monsters[MAX_MONSTER]{};
    CItem                 m_Items[MAX_ITEM]{};
};

class CServer
{
private:
    int                   m_RecentID{};                  // ���� �ֱٿ� �÷��̾�� ������ ���̵�

    SOCKET                m_ListenSocket{};              // Ŭ���̾�Ʈ�� �����ϱ� ���� ��� ����
    SOCKADDR_IN           m_SocketAddress{};             // ������ ���� �ּ� ����ü

    HANDLE                m_MainSyncHandles[2]{};        // �ֽ������� ����ȭ�� ���� �ڵ�(��Ī������ ���´�.)
    HANDLE                m_SyncHandles[MAX_PLAYER]{};   // �� �������Լ��� ����ȭ�� ���� �ڵ�

    CMap*                 m_Map{};                       // ���� ��
    GameData*             m_GameData{};                  // ���� ������

    int                   m_Round{ 1 };                  // ���� ����
    int                   m_PlayerCount{};               // ���� ������ �÷��̾� ��

    const float           m_MonsterGenTime{ 300.0f };    // ���Ͱ� �����Ǳ� ���� �����ؾ� �ϴ� �ð�
    float                 m_CurrentMonsterGenTime{};     // ���� ������ �����ð�
    int                   m_TotalMonsterCount{};         // �� ������ ��
    int                   m_CurrentMonsterCount{};       // ���� �ʵ����� �����ϴ� ������ ��

    const float           m_ItemGenTime{ 600.0f };       // �������� �����Ǳ� ���� �����ؾ� �ϴ� �ð�
    float                 m_CurrentItemGenTime{};        // ���� �������� �����ð�

public:
    CServer();
    ~CServer();

    void err_quit(const char* Msg);
    void err_display(const char* Msg);
    int recvn(SOCKET Socket, char* Buffer, int Length, int Flags);

    static DWORD WINAPI AcceptClient(LPVOID Arg);                       // Ŭ���̾�Ʈ ������ �޴� ������ �Լ�
    static DWORD WINAPI ProcessClient(LPVOID Arg);                      // �� Ŭ���̾�Ʈ ó���� ���� ������ �Լ�

    void InitServer();                                                  // ���� �ʱ�ȭ �� ���� ����
    void InitEvent();                                                   // �̺�Ʈ ��ü ����
    void BuildObject();                                                 // ���� �����ϰ�, ���� �� ��� ��ü�� ����/���� �� �ʱ�ȭ
    void InitWaitingScene();                                            // ���� �� ��ü �ʱ�ȭ
    void InitGameScene();                                               // ���� �� ��ü �ʱ�ȭ

    void ProcessGameData();                                             // �ۼ��� �����͸� ó���ϴ� �Լ�
    void WaitingLoop();                                                 // ���� ���� ����
    void GameLoop();                                                    // �ΰ��� ���� ����

    int GetValidID() const;                                             // ��밡���� �÷��̾� ���̵� ��ȯ

    bool CreatePlayer(SOCKET Socket, const SOCKADDR_IN& SocketAddress); // ������ Ŭ���̾�Ʈ�� �÷��̾� ����
    bool DestroyPlayer(int ID);                                         // ������ Ŭ���̾�Ʈ�� �÷��̾� ����

    bool CheckAllPlayerReady();                                         // ��� �÷��̾ �غ� �������� Ȯ���ϴ� �Լ�
    bool CheckGameOver();                                               // ���� ���� �������� Ȯ���ϴ� �Լ�

    void UpdateRound();                                                 // ���带 �����ϴ� �Լ�
    void CreateMonster();                                               // ���� �ֱ�� ���͸� �����ϴ� �Լ�
    void CreateItem();                                                  // ���� �ֱ�� �������� �����ϴ� �Լ�

    void SetMonstersTarget();                                           // ���� ��ü�� Ÿ���� �����Ͽ� ������ �����ϴ� �Լ�
    void Animate();                                                     // ���� �� ��� ��ü�� �������� ó���ϴ� �Լ�

    void CheckPlayerByMonsterCollision();                               // �÷��̾�� ���� �� �浹 �˻� �� ��ó��
    void CheckBulletByMonsterCollision();                               // �÷��̾��� �Ѿ˰� ���� �� �浹 �˻� �� ��ó��
    void CheckTowerByMonsterCollision();                                // Ÿ���� ���� �� �浹 �˻� �� ��ó��
    void CheckPlayerByItemCollision();                                  // �÷��̾�� ������ �� �浹 �˻� �� ��ó��
};
