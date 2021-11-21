#pragma once
#pragma comment (lib, "Msimg32.lib")
#pragma comment(lib, "ws2_32")

#include "targetver.h"

// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define WIN32_LEAN_AND_MEAN      

// Windows ��� ����
#include <winsock2.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ ��Ÿ�� ��� �����Դϴ�.
#include <iostream>
#include <fstream>
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>

using namespace std;

#define CLIENT_WIDTH	      800
#define CLIENT_HEIGHT		  600

#define PI				3.141592f

// �ؽ�Ʈ�� ���� HP ���
#define DEBUG_HP

struct POSITION
{
	float m_X{};
	float m_Y{};

	POSITION() = default;
	POSITION(float X, float Y) : m_X{ X }, m_Y{ Y } {}
};

struct USER_RECT
{
	int m_Left{};
	int m_Top{};
	int m_Width{};
	int m_Height{};
};

struct VECTOR2D
{
	float m_X{};
	float m_Y{};

	 VECTOR2D() = default;
	 VECTOR2D(float X, float Y) : m_X{ X }, m_Y{ Y } {}
};

float RandF(float Min, float Max);

void DrawRect(HDC hMemDC, const POSITION& Position, float Width, float Height, HDC hMemDC2, const USER_RECT& Rect, COLORREF TransparentColor);
void FixedDrawRect(HDC hMemDC, const POSITION& Position, float FixedWidth, float FixedHeight, float Width, float Height, HDC hMemDC2, const USER_RECT& Rect, COLORREF TransparentColor);
HBITMAP GetRotatedBitmap(HDC hDC, HBITMAP hBitmap, int SourceX, int SourceY, int DestWidth, int DestHeight, float Angle, COLORREF TransparentColor);
