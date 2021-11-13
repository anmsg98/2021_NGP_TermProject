#include "stdafx.h"
#include "Timer.h"

void CTimer::Start()
{
	QueryPerformanceFrequency(&m_Second);
	QueryPerformanceCounter(&m_LastTime);
}

void CTimer::Update()
{
	QueryPerformanceCounter(&m_CurrentTime);

	m_DeltaTime = (float)(m_CurrentTime.QuadPart - m_LastTime.QuadPart) / m_Second.QuadPart;
	m_LastTime = m_CurrentTime;
}

float CTimer::GetDeltaTime()
{
	return m_DeltaTime;
}
