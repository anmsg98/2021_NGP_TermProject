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


// 인수목록
// HDC hdc :
// HBITMAP hBitmap :
// int source_x : 회전 이미지 비트맵에서의, 출력할 사각영역의 좌상단x좌표.
// int source_y : 회전 이미지 비트맵에서의, 출력할 사각영역의 좌상단y좌표.
// int dest_width : 회전할 비트맵 이미지의 폭 크기 == 회전할 비트맵 이미지가 출력될 폭 크기
// int dest_heigjt : 회전할 비트맵 이미지의 높이 크기 == 회전할 비트맵 이미지가 출력될 높이 크기
// float angle : 회전할 각도.
// COLORREF bkColor : 회전후 생긴 공백영역을 칠할 색상. 이미지 사각형을 회전하면, 공백영역이 생깁니다. (예)다이아몬드 형태로 회전된 사각형에서의 뒤부분 영역

HBITMAP GetRotatedBitmap(HDC hdc, HBITMAP hBitmap, int source_x, int source_y,
	                     int dest_width, int dest_height, float angle, COLORREF bkColor)
{
	HDC sourceDC = CreateCompatibleDC(hdc); // 회전할 비트맵 원본을 선택할 DC
	HDC destDC = CreateCompatibleDC(hdc); // 회전할 비트맵을 출력받을 DC
	HBITMAP hbmResult = CreateCompatibleBitmap(hdc, dest_width, dest_height); // 회전할 비트맵을 출력받을 메모리비트맵 핸들

	HBITMAP hbmOldSource = (HBITMAP)SelectObject(sourceDC, hBitmap); // 회전할 비트맵 원본을 DC에 선택
	HBITMAP hbmOldDest = (HBITMAP)SelectObject(destDC, hbmResult); // 회전할 비트맵을 출력받을 DC에, 회전할 비트맵을 출력받을 메모리비트맵 선택

	HBRUSH hbrBack = CreateSolidBrush(bkColor); // 회전으로 인한, 공백영역을 칠할 브러시핸들 생성
	HBRUSH hbrOld = (HBRUSH)SelectObject(destDC, hbrBack); // 브러시핸들 선택
	PatBlt(destDC, 0, 0, dest_width, dest_height, PATCOPY); // 선택된 브러시로, 회전할 비트맵을 출력받을 DC에, 미리 색상을 채워 지움
	DeleteObject(SelectObject(destDC, hbrOld)); // 브러시 해제

	angle = (float)(angle * PI / 180); // 회전할 각도(360분법)를, 라디안단위의 각도로 변경
	float cosine = (float)cos(angle); // 회전이동변환 행렬에 들어갈 cos세타 값을 구함
	float sine = (float)sin(angle); // 회전이동변환 행렬에 들어갈 sin세타 값을 구함

	SetGraphicsMode(destDC, GM_ADVANCED); // 윈도우 좌표계의 회전을 위하여, 그래픽모드를 확장모드로 변경합니다.(요구사항:윈98,NT이상)

	// 회전이동변환 행렬을 정의.
	// (윈도우즈 API정복 개정판 2권 Page(1279) 참조.)
	// (게임 프로그래머를 위한 기초수학과 물리 Page(196) 참조.)

	XFORM xform; // 방정식을 표현하는 3행3열의 행렬 선언
	xform.eM11 = cosine; // 1행 1열 성분 설정 (회전성분)
	xform.eM12 = sine; // 1행 2열 성분 설정 (회전성분)
	xform.eM21 = -sine; // 2행 1열 성분 설정 (회전성분)
	xform.eM22 = cosine; // 2행 2열 성분 설정 (회전성분)
	xform.eDx = (FLOAT)dest_width / 2.0f; // 3행 1열 성분 설정 (X축 이동 성분)
	xform.eDy = (FLOAT)dest_height / 2.0f; // 3행 2열 성분 설정 (Y축 이동 성분)

		// 설정한 행렬로, 인수DC의 윈도우즈 좌표계를 변환.
		// 인수DC는 출력대상으로 메모리비트맵을 가지므로,
		// 선택하고 있는 메모리비트맵의 좌표체계가 회전된다.

	SetWorldTransform(destDC, &xform);

	// 회전된 메모리DC에, 회전할 이미지를 출력

	//BitBlt(destDC, -(dest_width / 2), -(dest_height / 2), dest_width, dest_height, sourceDC, source_x, source_y, SRCCOPY);
	StretchBlt(destDC, -(dest_width / 4), -(dest_height / 4), dest_width * 0.5, dest_height * 0.5, sourceDC, source_x, source_y, dest_width, dest_height, SRCCOPY);

	// 사용된 자원 해제

	SelectObject(sourceDC, hbmOldSource);
	SelectObject(destDC, hbmOldDest);
	DeleteObject(sourceDC);
	DeleteObject(destDC);

	// 회전된 비트맵 핸들 리턴

	return hbmResult;
}

