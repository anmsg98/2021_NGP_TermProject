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
	HDC SourceDC{ CreateCompatibleDC(hDC) };									 // ȸ���� ��Ʈ�� ������ ������ DC
	HDC DestDC{ CreateCompatibleDC(hDC) };										 // ȸ���� ��Ʈ���� ��� ���� DC

	HBITMAP hResultBitmap{ CreateCompatibleBitmap(hDC, DestWidth, DestHeight) }; // ȸ���� ��Ʈ���� ��� ���� �޸� ��Ʈ�� �ڵ�
	HBITMAP hSourceBitmap{ (HBITMAP)SelectObject(SourceDC, hBitmap) };			 // ȸ���� ��Ʈ�� ������ DC�� ����
	HBITMAP hDestBitmap{ (HBITMAP)SelectObject(DestDC, hResultBitmap) };		 // ȸ���� ��Ʈ���� ��¹��� DC��, ȸ���� ��Ʈ���� ��¹��� �޸� ��Ʈ�� ����

	HBRUSH hBrush{ CreateSolidBrush(TransparentColor) };						 // ȸ������ ����, ���鿵���� ĥ�� �귯���ڵ� ����
	HBRUSH hOldBrush{ (HBRUSH)SelectObject(DestDC, hBrush) };			

	PatBlt(DestDC, 0, 0, DestWidth, DestHeight, PATCOPY);						 // ���õ� �귯�÷� ȸ���� ��Ʈ���� ��¹��� DC��, �̸� ������ ä�� ����
	SelectObject(DestDC, hOldBrush);
	DeleteObject(hBrush);

	Angle = Angle * PI / 180.0f; 

	float Cosine{ cosf(Angle) };
	float Sine{ sinf(Angle) };

	SetGraphicsMode(DestDC, GM_ADVANCED);									     // ������ ��ǥ���� ȸ���� ���Ͽ�, �׷��ȸ�带 Ȯ����� ����

	XFORM Xform{ Cosine, Sine, -Sine, Cosine, 0.5f * (float)DestWidth, 0.5f * (float)DestHeight };

	// ������ ��ķ�, �μ�DC�� �������� ��ǥ�踦 ��ȯ
	// �μ�DC�� ��´������ �޸𸮺�Ʈ���� �����Ƿ�, �����ϰ� �ִ� �޸𸮺�Ʈ���� ��ǥü�谡 ȸ���ȴ�.
	SetWorldTransform(DestDC, &Xform);

	// ȸ���� �޸� DC��, ȸ���� �̹����� ���
	StretchBlt(DestDC, -(int)(0.25f * DestWidth), -(int)(0.25f * DestHeight), (int)(0.5f * DestWidth), (int)(0.5f * DestHeight), SourceDC, SourceX, SourceY, DestWidth, DestHeight, SRCCOPY);

	SelectObject(SourceDC, hSourceBitmap);
	SelectObject(DestDC, hDestBitmap);
	DeleteObject(SourceDC);
	DeleteObject(DestDC);

	return hResultBitmap;
}
