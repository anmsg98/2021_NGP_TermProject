#pragma once

class CTimer
{
private:
	LARGE_INTEGER		m_StartTime{};
	LARGE_INTEGER		m_EndTime{};
	LARGE_INTEGER		m_Second{};

public:
	CTimer();
	~CTimer() = default;

	void Start();
	void End();

	void Wait(float LockFPS);
};
