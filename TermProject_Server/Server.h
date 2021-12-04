#pragma once
#include "Player.h"
#include "Tower.h"
#include "Monster.h"
#include "Item.h"

class CTimer;
class CMap;

enum GAME_STATE { WAITING, INGAME };
enum MONSTER_GEN_LOCATION { TOP, BOTTOM, LEFT, RIGHT };

struct GameData
{
    int                   m_State{ WAITING };
    bool                  m_GameOver{};

    int                   m_Round{ 1 };
    int                   m_ScheduledGenTime{};          // ������ �������� �����ð�
    int                   m_TotalMonsterCount{};         // �̹� ���忡 ������ �� ������ ��
    int                   m_CurrentMonsterCount{};       // ���� �ʵ����� �����ϴ� ������ ��

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

    CTimer*               m_Timer{};                     // ��Ŷ �ۼ��� �ӵ��� �����ϱ� ���� Ÿ�̸�

    CMap*                 m_Map{};                       // ���� ��
    GameData*             m_GameData{};                  // ���� ������

    int                   m_PlayerCount{};               // ���� ������ �÷��̾� ��

    const float           m_MonsterGenTime{ 600.0f };    // ���Ͱ� �����Ǳ� ���� �����ؾ� �ϴ� �ð�
    float                 m_CurrentMonsterGenTime{};     // ���� ������ �����ð�

    const float           m_ItemGenTime{ 600.0f };       // �������� �����Ǳ� ���� �����ؾ� �ϴ� �ð�
    float                 m_CurrentItemGenTime{};        // ���� �������� �����ð�

    const float           m_ResultTime{ 600.0f };        // ���â�� ��µǴ� �� �ð�
    float                 m_CurrentResultTime{};         // ���� ���â�� ��½ð�

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
    void CheckGameOver();                                               // ���� ���� �������� Ȯ���ϴ� �Լ�

    void UpdateRound();                                                 // ���带 �����ϴ� �Լ�
    void CreateMonster();                                               // ���� �ֱ�� ���͸� �����ϴ� �Լ�
    void CreateItem();                                                  // ���� �ֱ�� �������� �����ϴ� �Լ�

    void SetMonstersTarget();                                           // ���� ��ü�� Ÿ���� �����Ͽ� ������ �����ϴ� �Լ�
    void Animate();                                                     // ���� �� ��� ��ü�� �������� ó���ϴ� �Լ�

    void CheckCollision();                                              // ���� ���� ��� �浹 �˻� �� ��ó��
    void CheckPlayerByMonsterCollision();                               // �÷��̾�� ���� �� �浹 �˻� �� ��ó��
    void CheckBulletByMonsterCollision();                               // �÷��̾��� �Ѿ˰� ���� �� �浹 �˻� �� ��ó��
    void CheckTowerByMonsterCollision();                                // Ÿ���� ���� �� �浹 �˻� �� ��ó��
    void CheckPlayerByItemCollision();                                  // �÷��̾�� ������ �� �浹 �˻� �� ��ó��
};
