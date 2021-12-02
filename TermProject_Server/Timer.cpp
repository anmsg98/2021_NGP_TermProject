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

#ifdef DEBUG_PRINT_FPS
	cout << "FPS : " << (float)(m_EndTime.QuadPart - m_StartTime.QuadPart) / m_Second.QuadPart << endl;
#endif
}