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
    int                   m_RecentID{};                  // 가장 최근에 플레이어에게 설정된 아이디

    SOCKET                m_ListenSocket{};              // 클라이언트를 수용하기 위한 대기 소켓
    SOCKADDR_IN           m_SocketAddress{};             // 서버의 소켓 주소 구조체

    HANDLE                m_MainSyncHandles[2]{};        // 주스레드의 동기화를 위한 핸들(대칭구조를 갖는다.)
    HANDLE                m_SyncHandles[MAX_PLAYER]{};   // 각 스레드함수의 동기화를 위한 핸들

    CMap*                 m_Map{};                       // 게임 맵
    GameData*             m_GameData{};                  // 게임 데이터

    int                   m_Round{ 1 };                  // 현재 라운드
    int                   m_PlayerCount{};               // 현재 접속한 플레이어 수

    const float           m_MonsterGenTime{ 300.0f };    // 몬스터가 생성되기 위해 도달해야 하는 시간
    float                 m_CurrentMonsterGenTime{};     // 현재 몬스터의 생성시간
    int                   m_TotalMonsterCount{};         // 총 몬스터의 수
    int                   m_CurrentMonsterCount{};       // 현재 필드위에 존재하는 몬스터의 수

    const float           m_ItemGenTime{ 600.0f };       // 아이템이 생성되기 위해 도달해야 하는 시간
    float                 m_CurrentItemGenTime{};        // 현재 아이템의 생성시간

public:
    CServer();
    ~CServer();

    void err_quit(const char* Msg);
    void err_display(const char* Msg);
    int recvn(SOCKET Socket, char* Buffer, int Length, int Flags);

    static DWORD WINAPI AcceptClient(LPVOID Arg);                       // 클라이언트 접속을 받는 쓰레드 함수
    static DWORD WINAPI ProcessClient(LPVOID Arg);                      // 각 클라이언트 처리를 위한 쓰레드 함수

    void InitServer();                                                  // 윈속 초기화 및 서버 생성
    void InitEvent();                                                   // 이벤트 객체 생성
    void BuildObject();                                                 // 맵을 생성하고, 게임 내 모든 객체의 가로/세로 값 초기화
    void InitWaitingScene();                                            // 대기실 씬 객체 초기화
    void InitGameScene();                                               // 게임 씬 객체 초기화

    void ProcessGameData();                                             // 송수신 데이터를 처리하는 함수
    void WaitingLoop();                                                 // 대기실 게임 루프
    void GameLoop();                                                    // 인게임 게임 루프

    int GetValidID() const;                                             // 사용가능한 플레이어 아이디 반환

    bool CreatePlayer(SOCKET Socket, const SOCKADDR_IN& SocketAddress); // 수용한 클라이언트의 플레이어 생성
    bool DestroyPlayer(int ID);                                         // 수용한 클라이언트의 플레이어 제거

    bool CheckAllPlayerReady();                                         // 모든 플레이어가 준비 상태인지 확인하는 함수
    bool CheckGameOver();                                               // 게임 오버 상태인지 확인하는 함수

    void UpdateRound();                                                 // 라운드를 갱신하는 함수
    void CreateMonster();                                               // 일정 주기로 몬스터를 생성하는 함수
    void CreateItem();                                                  // 일정 주기로 아이템을 생성하는 함수

    void SetMonstersTarget();                                           // 몬스터 객체의 타겟을 설정하여 방향을 결정하는 함수
    void Animate();                                                     // 게임 내 모든 객체의 움직임을 처리하는 함수

    void CheckPlayerByMonsterCollision();                               // 플레이어와 몬스터 간 충돌 검사 및 후처리
    void CheckBulletByMonsterCollision();                               // 플레이어의 총알과 몬스터 간 충돌 검사 및 후처리
    void CheckTowerByMonsterCollision();                                // 타워와 몬스터 간 충돌 검사 및 후처리
    void CheckPlayerByItemCollision();                                  // 플레이어와 아이템 간 충돌 검사 및 후처리
};
