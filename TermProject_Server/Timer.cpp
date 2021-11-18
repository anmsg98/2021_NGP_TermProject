#include "stdafx.h"
#include "Timer.h"

void CTimer::Start()
{
	QueryPerformanceFrequency(&m_Second);
	QueryPerformanceCounter(&m_LastTime);
}

void CTimer::Update(float LockFPS)
{
	QueryPerformanceCounter(&m_CurrentTime);

	m_DeltaTime = (float)(m_CurrentTime.QuadPart - m_LastTime.QuadPart) / m_Second.QuadPart;
	m_LastTime = m_CurrentTime;

	if (LockFPS > 0.0f)
	{
		while (m_DeltaTime < (1.0f / LockFPS))
		{
			QueryPerformanceCounter(&m_CurrentTime);
			m_DeltaTime = (float)(m_CurrentTime.QuadPart - m_LastTime.QuadPart) / m_Second.QuadPart;
		}
	}
}

float CTimer::GetDeltaTime()
{
	return m_DeltaTime;
}
