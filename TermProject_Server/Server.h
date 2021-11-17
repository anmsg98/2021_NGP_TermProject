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
	static int				m_RecentID;						// ���� �ֱٿ� �÷��̾�� ������ ���̵�

	SOCKET					m_ListenSocket{};				// Ŭ���̾�Ʈ�� �����ϱ� ���� ��� ����
	SOCKADDR_IN				m_SocketAddress{};				// ������ ���� �ּ� ����ü

	CTimer*					m_Timer{};						// ���� ���� �ð��� �����ϱ� ���� Ÿ�̸�

	HANDLE					m_MainSyncHandle{};				// �ֽ������� ����ȭ�� ���� �ڵ�
	HANDLE					m_SyncHandles[MAX_PLAYER]{};	// �������Լ��� ����ȭ�� ���� �ڵ�

	CMap*					m_Map{};
	GameData*				m_GameData{};					// ���� ������

public:
	CServer();
	~CServer();

	// Ŭ���̾�Ʈ ������ �޴� ������ �Լ�
	static DWORD WINAPI AcceptClient(LPVOID Arg);

	// �� Ŭ���̾�Ʈ ó���� ���� ������ �Լ�
	static DWORD WINAPI ProcessClient(LPVOID Arg);

	// �������� �̷������ ó���� ���� �Լ�
	void ProcessGameData();

	// ���� ���� �����Լ�
    void err_quit(const char* Msg);
	void err_display(const char* Msg);

	// Length ����Ʈ�� Ȯ���ϰ� ����
	int recvn(SOCKET Socket, char* Buffer, int Length, int Flags);

	// ���� �ʱ�ȭ �� ���� ����
	void InitServer();

	// �̺�Ʈ ����
	void InitEvent();

	// ��ȿ�� �÷��̾� ���̵� ��ȯ
	int GetValidID() const;
	
	// ������ Ŭ���̾�Ʈ�� �÷��̾� ���� �� ����
	bool CreatePlayer(SOCKET Socket, const SOCKADDR_IN& SocketAddress);
	bool DestroyPlayer(int ID);

	// ���� Ȯ��
	bool CheckAllPlayerReady();
	bool CheckGameOver();

	void WaitingLoop();
	void GameLoop();

	void BuildObject();

	void Animate();

	void CreateMonster();
	void CreateItem();

	// ���� ���� ���� ��ü �� �浹 �˻�
	void CheckBulletByMonsterCollision();
	void CheckTowerByMonsterCollision();
	void CheckPlayerByItemCollision();
};
