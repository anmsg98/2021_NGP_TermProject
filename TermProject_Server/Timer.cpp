#include "stdafx.h"
#include "Timer.h"

CTimer::CTimer()
{
	QueryPerformanceFrequency(&m_Second);
}

void CTimer::Start()
{
	QueryPerformanceCounter(&m_StartTime);
}

void CTimer::End()
{
	QueryPerformanceCounter(&m_EndTime);
}

void CTimer::Wait(float LockFPS)
{
	if (LockFPS > 0.0f)
	{
		while ((float)(m_EndTime.QuadPart - m_StartTime.QuadPart) / m_Second.QuadPart < (1.0f / LockFPS))
		{
			QueryPerformanceCounter(&m_EndTime);
		}
	}

	m_FrameRate = (int)(1.0f / ((float)(m_EndTime.QuadPart - m_StartTime.QuadPart) / m_Second.QuadPart) + 0.5f);

#ifdef DEBUG_PRINT_FPS
	cout << "FPS : " << m_FrameRate << endl;
#endif
}

int CTimer::GetFrameRate() const
{
	return m_FrameRate;
}
