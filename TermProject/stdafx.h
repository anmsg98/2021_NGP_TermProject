#pragma once
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")


#include "targetver.h"

// 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define WIN32_LEAN_AND_MEAN      

// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ 런타임 헤더 파일입니다.
#include <stack>

using namespace std;

#define CLIENT_WIDTH	 800
#define CLIENT_HEIGHT	 600

struct POSITION
{
	float m_X;
	float m_Y;

	POSITION() = default;
	POSITION(float X, float Y) :
		m_X{ X }, m_Y{ Y }
	{

	}
};

void DrawRectangle(const HDC& hDC, const POSITION& Position, float Width, float Height);
