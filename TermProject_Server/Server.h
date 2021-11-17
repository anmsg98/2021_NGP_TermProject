#pragma once
#include "Timer.h"
#include "Player.h"
#include "Tower.h"
#include "Monster.h"
#include "Item.h"

#define SERVER_CAPACITY			   2
#define SERVER_PORT				9000

class CMap;

enum MONSTER_GEN_LOCATION { TOP, BOTTOM, LEFT, RIGHT };

struct GameData
{
	CPlayer					m_Players[MAX_PLAYER]{};
	CTower					m_Tower{};
	CMonster				m_Monsters[MAX_MONSTER]{};
	CItem					m_Items[MAX_ITEM]{};
};

class CServer
{
private:			
	static int				m_RecentID;						// 가장 최근에 플레이어에게 설정된 아이디

	SOCKET					m_ListenSocket{};				// 클라이언트를 수용하기 위한 대기 소켓
	SOCKADDR_IN				m_SocketAddress{};				// 서버의 소켓 주소 구조체

	CTimer*					m_Timer{};						// 게임 내의 시간을 관리하기 위한 타이머

	HANDLE					m_MainSyncHandle{};				// 주스레드의 동기화를 위한 핸들
	HANDLE					m_SyncHandles[MAX_PLAYER]{};	// 스레드함수의 동기화를 위한 핸들

	CMap*					m_Map{};
	GameData*				m_GameData{};					// 게임 데이터

public:
	CServer();
	~CServer();

	// 클라이언트 접속을 받는 쓰레드 함수
	static DWORD WINAPI AcceptClient(LPVOID Arg);

	// 각 클라이언트 처리를 위한 쓰레드 함수
	static DWORD WINAPI ProcessClient(LPVOID Arg);

	// 서버에서 이루어지는 처리를 위한 함수
	void ProcessGameData();

	// 소켓 관련 오류함수
    void err_quit(const char* Msg);
	void err_display(const char* Msg);

	// Length 바이트를 확실하게 수신
	int recvn(SOCKET Socket, char* Buffer, int Length, int Flags);

	// 윈속 초기화 및 서버 생성
	void InitServer();

	// 이벤트 생성
	void InitEvent();

	// 유효한 플레이어 아이디 반환
	int GetValidID() const;
	
	// 수용한 클라이언트의 플레이어 생성 및 제거
	bool CreatePlayer(SOCKET Socket, const SOCKADDR_IN& SocketAddress);
	bool DestroyPlayer(int ID);

	// 상태 확인
	bool CheckAllPlayerReady();
	bool CheckGameOver();

	void WaitingLoop();
	void GameLoop();

	void BuildObject();

	void Animate();

	void CreateMonster();
	void CreateItem();

	// 게임 월드 내의 객체 간 충돌 검사
	void CheckBulletByMonsterCollision();
	void CheckTowerByMonsterCollision();
	void CheckPlayerByItemCollision();
};
