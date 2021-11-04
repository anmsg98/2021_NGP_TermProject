﻿#pragma once
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#pragma comment (lib, "Msimg32.lib")

#include "targetver.h"
#include "Resource.h"

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
#include <iostream>
#include <fstream>
#include <stack>
#include <unordered_map>
#include <string>

using namespace std;

#define CLIENT_WIDTH	 800
#define CLIENT_HEIGHT	 600

struct POSITION
{
	float m_X;
	float m_Y;

	POSITION() = default;
	POSITION(float X, float Y) : m_X{ X }, m_Y{ Y } {}
};

struct LTWH
{
	int m_Left;
	int m_Top;
	int m_Width;
	int m_Height;
};

void DrawRectangle(const HDC& hDC, const POSITION& Position, float Width, float Height);
