#include "stdafx.h"
#include "SoundManager.h"

CSoundManager* CSoundManager::m_Instance;

CSoundManager::~CSoundManager()
{
	if (m_Instance)
	{
		m_System->release();

		delete m_Instance;
	}
}

CSoundManager* CSoundManager::GetInstance()
{
	if (!m_Instance)
	{
		m_Instance = new CSoundManager{};
	}

	return m_Instance;
}

void CSoundManager::Init()
{
	m_Result = System_Create(&m_System);

	if (m_Result != FMOD_OK)
	{
		return;
	}

	m_Result = m_System->init(MAX_SOUND, FMOD_INIT_NORMAL, nullptr);

	if (m_Result != FMOD_OK)
	{
		return;
	}

	// 추가할 사운드 파일
	m_Result = m_System->createSound("Sound/Waiting_BGM.mp3", FMOD_LOOP_NORMAL, 0, &m_Sound[TITLE_BACKGROUND_SOUND]);
	m_Result = m_System->createSound("Sound/InGame_BGM.mp3", FMOD_LOOP_NORMAL, 0, &m_Sound[GAME_BACKGROUND_SOUND]);
	m_Result = m_System->createSound("Sound/Button.wav", FMOD_LOOP_OFF, 0, &m_Sound[BUTTON_SOUND]);
	m_Result = m_System->createSound("Sound/Bullet.wav", FMOD_LOOP_OFF, 0, &m_Sound[BULLET_SOUND]);

	if (m_Result != FMOD_OK)
	{
		return;
	}
}

void CSoundManager::Play(int Index, float Volume)
{
	m_System->update();
	m_Result = m_System->playSound(FMOD_CHANNEL_FREE, m_Sound[Index], false, &m_Channel[Index]);
	m_Channel[Index]->setVolume(Volume);
}

void CSoundManager::Stop(int Index)
{
	m_Channel[Index]->stop();
}

void CSoundManager::Pause(int Index)
{
	m_Channel[Index]->setPaused(true);
}

void CSoundManager::Resume(int Index)
{
	m_Channel[Index]->setPaused(false);
}
