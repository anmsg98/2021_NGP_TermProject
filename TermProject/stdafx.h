#pragma once
#pragma comment (lib, "Msimg32.lib")
#pragma comment(lib, "ws2_32")

#include "targetver.h"
#include "Resource.h"

// 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define WIN32_LEAN_AND_MEAN

// Windows 헤더 파일
#include <winsock2.h>

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
#include <vector>
#include <string>
#include <cmath>
#include <ctime>

using namespace std;

// FMOD 사운드 헤더 파일입니다.
#include "fmod.hpp"

using namespace FMOD;

#define CLIENT_WIDTH	      800
#define CLIENT_HEIGHT		  600

#define PI				3.141592f

// 텍스트로 현재 HP 출력
//#define DEBUG_HP

struct VECTOR2D
{
	float m_X{};
	float m_Y{};

	VECTOR2D() = default;
	VECTOR2D(float X, float Y) : m_X{ X }, m_Y{ Y } {}

	VECTOR2D operator+(const VECTOR2D& Rhs)
	{
		VECTOR2D Result;

		Result.m_X = m_X + Rhs.m_X;
		Result.m_Y = m_Y + Rhs.m_Y;

		return Result;
	}

	VECTOR2D operator-(const VECTOR2D& Rhs)
	{
		VECTOR2D Result;

		Result.m_X = m_X - Rhs.m_X;
		Result.m_Y = m_Y - Rhs.m_Y;

		return Result;
	}
};

struct USER_RECT
{
	int m_Left{};
	int m_Top{};
	int m_Width{};
	int m_Height{};
};

float RandF(float Min, float Max);

void DrawRect(HDC hMemDC, const VECTOR2D& Position, float Width, float Height, HDC hMemDC2, const USER_RECT& Rect, COLORREF TransparentColor);
void FixedDrawRect(HDC hMemDC, const VECTOR2D& Position, float FixedWidth, float FixedHeight, float Width, float Height, HDC hMemDC2, const USER_RECT& Rect, COLORREF TransparentColor);
HBITMAP GetRotatedBitmap(HDC hDC, HBITMAP hBitmap, int SourceX, int SourceY, int DestWidth, int DestHeight, float Angle, COLORREF TransparentColor);

namespace Vector
{
	float Distance(const VECTOR2D& Position1, const VECTOR2D& Position2);
	float Length(const VECTOR2D& Vector);
	VECTOR2D Inverse(const VECTOR2D& Vector);
}
