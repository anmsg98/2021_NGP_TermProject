#pragma once
#pragma comment(lib, "ws2_32")

#include "targetver.h"

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
#include <cmath>
#include <ctime>

using namespace std;

// 서버 정보
#define SERVER_PORT            9000
#define SERVER_LOCK_FPS       60.0f

// 게임 내 객체 수 정보
#define MAX_PLAYER				  4
#define MAX_BULLET				  5
#define MAX_MONSTER			     30
#define MAX_ITEM				  5

// 콘솔창에 디버그 정보 출력
#define DEBUG_PRINT_FPS
//#define DEBUG_PRINT_MONSTER_GEN_POS
//#define DEBUG_PRINT_ITEM_GEN_POS
//#define DEBUG_PRINT_ROUND
//#define DEBUG_PRINT_WAVE

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

namespace Vector
{
	float Distance(const VECTOR2D& Position1, const VECTOR2D& Position2);
	float Length(const VECTOR2D& Vector);
	VECTOR2D Inverse(const VECTOR2D& Vector);
}
