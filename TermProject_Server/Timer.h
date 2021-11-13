#pragma once

class CTimer
{
private:
	LARGE_INTEGER		m_Second{};
	LARGE_INTEGER		m_LastTime{};
	LARGE_INTEGER		m_CurrentTime{};

	float				m_DeltaTime{};

public:
	CTimer() = default;
	~CTimer() = default;

	void Start();
	void Update();

	float GetDeltaTime();
};
