#pragma once
#define MAX_SOUND 8

class CSoundManager
{
public:
	enum
	{
		WAITING_BACKGROUND_SOUND,
		GAME_BACKGROUND_SOUND,
		BUTTON_SOUND,
		BULLET_SOUND,
		MONSTER_DEAD_SOUND,
		EXPLOSION_SOUND,
		ITEM_DROP_SOUND,
		RESULT_SOUND
	};

private:
	static CSoundManager*		m_Instance;

	System*						m_System{};
	Channel*					m_Channel[MAX_SOUND]{};
	Sound*						m_Sound[MAX_SOUND]{};
	FMOD_RESULT					m_Result{};

private:
	CSoundManager() = default;
	~CSoundManager();

public:
	static CSoundManager* GetInstance();

	void Init();
	void Play(int Index, float Volume);
	void Stop(int Index);
	void Pause(int Index);
	void Resume(int nIndex);
};
