#include "stdafx.h"

float RandF(float Min, float Max)
{
	return Min + ((float)rand() / (float)RAND_MAX) * (Max - Min);
}

void DrawRect(HDC hMemDC, const POSITION& Position, float Width, float Height, HDC hMemDC2, const USER_RECT& Rect, COLORREF TransparentColor)
{
	int OriginX{ (int)(Position.m_X - 0.5f * Width) };
	int OriginY{ (int)(Position.m_Y - 0.5f * Height) };

	TransparentBlt(hMemDC, OriginX, OriginY, (int)Width, (int)Height,
				   hMemDC2, (int)Rect.m_Left, (int)Rect.m_Top, (int)Rect.m_Width, (int)Rect.m_Height, TransparentColor);
}

void FixedDrawRect(HDC hMemDC, const POSITION& Position, float FixedWidth, float FixedHeight, float Width, float Height, HDC hMemDC2, const USER_RECT& Rect, COLORREF TransparentColor)
{
	int OriginX{ (int)(Position.m_X - 0.5f * FixedWidth) };
	int OriginY{ (int)(Position.m_Y - 0.5f * FixedHeight) };

	TransparentBlt(hMemDC, OriginX, OriginY, (int)Width, (int)Height,
				   hMemDC2, (int)Rect.m_Left, (int)Rect.m_Top, (int)Rect.m_Width, (int)Rect.m_Height, TransparentColor);
}

HBITMAP GetRotatedBitmap(HDC hDC, HBITMAP hBitmap, int SourceX, int SourceY, int DestWidth, int DestHeight, float Angle, COLORREF TransparentColor)
{
	HDC SourceDC{ CreateCompatibleDC(hDC) };									 // 회전할 비트맵 원본을 선택할 DC
	HDC DestDC{ CreateCompatibleDC(hDC) };										 // 회전할 비트맵을 출력 받을 DC

	HBITMAP hResultBitmap{ CreateCompatibleBitmap(hDC, DestWidth, DestHeight) }; // 회전할 비트맵을 출력 받을 메모리 비트맵 핸들
	HBITMAP hSourceBitmap{ (HBITMAP)SelectObject(SourceDC, hBitmap) };			 // 회전할 비트맵 원본을 DC에 선택
	HBITMAP hDestBitmap{ (HBITMAP)SelectObject(DestDC, hResultBitmap) };		 // 회전할 비트맵을 출력받을 DC에, 회전할 비트맵을 출력받을 메모리 비트맵 선택

	HBRUSH hBrush{ CreateSolidBrush(TransparentColor) };						 // 회전으로 인한, 공백영역을 칠할 브러시핸들 생성
	HBRUSH hOldBrush{ (HBRUSH)SelectObject(DestDC, hBrush) };			

	PatBlt(DestDC, 0, 0, DestWidth, DestHeight, PATCOPY);						 // 선택된 브러시로 회전할 비트맵을 출력받을 DC에, 미리 색상을 채워 지움
	SelectObject(DestDC, hOldBrush);
	DeleteObject(hBrush);

	Angle = Angle * PI / 180.0f; 

	float Cosine{ cosf(Angle) };
	float Sine{ sinf(Angle) };

	SetGraphicsMode(DestDC, GM_ADVANCED);									     // 윈도우 좌표계의 회전을 위하여, 그래픽모드를 확장모드로 변경

	XFORM Xform{ Cosine, Sine, -Sine, Cosine, 0.5f * (float)DestWidth, 0.5f * (float)DestHeight };

	// 설정한 행렬로, 인수DC의 윈도우즈 좌표계를 변환
	// 인수DC는 출력대상으로 메모리비트맵을 가지므로, 선택하고 있는 메모리비트맵의 좌표체계가 회전된다.
	SetWorldTransform(DestDC, &Xform);

	// 회전된 메모리 DC에, 회전할 이미지를 출력
	StretchBlt(DestDC, -(int)(0.25f * DestWidth), -(int)(0.25f * DestHeight), (int)(0.5f * DestWidth), (int)(0.5f * DestHeight), SourceDC, SourceX, SourceY, DestWidth, DestHeight, SRCCOPY);

	SelectObject(SourceDC, hSourceBitmap);
	SelectObject(DestDC, hDestBitmap);
	DeleteObject(SourceDC);
	DeleteObject(DestDC);

	return hResultBitmap;
}
