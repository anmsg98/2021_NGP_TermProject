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


// �μ����
// HDC hdc :
// HBITMAP hBitmap :
// int source_x : ȸ�� �̹��� ��Ʈ�ʿ�����, ����� �簢������ �»��x��ǥ.
// int source_y : ȸ�� �̹��� ��Ʈ�ʿ�����, ����� �簢������ �»��y��ǥ.
// int dest_width : ȸ���� ��Ʈ�� �̹����� �� ũ�� == ȸ���� ��Ʈ�� �̹����� ��µ� �� ũ��
// int dest_heigjt : ȸ���� ��Ʈ�� �̹����� ���� ũ�� == ȸ���� ��Ʈ�� �̹����� ��µ� ���� ũ��
// float angle : ȸ���� ����.
// COLORREF bkColor : ȸ���� ���� ���鿵���� ĥ�� ����. �̹��� �簢���� ȸ���ϸ�, ���鿵���� ����ϴ�. (��)���̾Ƹ�� ���·� ȸ���� �簢�������� �ںκ� ����

HBITMAP GetRotatedBitmap(HDC hdc, HBITMAP hBitmap, int source_x, int source_y,
	                     int dest_width, int dest_height, float angle, COLORREF bkColor)
{
	HDC sourceDC = CreateCompatibleDC(hdc); // ȸ���� ��Ʈ�� ������ ������ DC
	HDC destDC = CreateCompatibleDC(hdc); // ȸ���� ��Ʈ���� ��¹��� DC
	HBITMAP hbmResult = CreateCompatibleBitmap(hdc, dest_width, dest_height); // ȸ���� ��Ʈ���� ��¹��� �޸𸮺�Ʈ�� �ڵ�

	HBITMAP hbmOldSource = (HBITMAP)SelectObject(sourceDC, hBitmap); // ȸ���� ��Ʈ�� ������ DC�� ����
	HBITMAP hbmOldDest = (HBITMAP)SelectObject(destDC, hbmResult); // ȸ���� ��Ʈ���� ��¹��� DC��, ȸ���� ��Ʈ���� ��¹��� �޸𸮺�Ʈ�� ����

	HBRUSH hbrBack = CreateSolidBrush(bkColor); // ȸ������ ����, ���鿵���� ĥ�� �귯���ڵ� ����
	HBRUSH hbrOld = (HBRUSH)SelectObject(destDC, hbrBack); // �귯���ڵ� ����
	PatBlt(destDC, 0, 0, dest_width, dest_height, PATCOPY); // ���õ� �귯�÷�, ȸ���� ��Ʈ���� ��¹��� DC��, �̸� ������ ä�� ����
	DeleteObject(SelectObject(destDC, hbrOld)); // �귯�� ����

	angle = (float)(angle * PI / 180); // ȸ���� ����(360�й�)��, ���ȴ����� ������ ����
	float cosine = (float)cos(angle); // ȸ���̵���ȯ ��Ŀ� �� cos��Ÿ ���� ����
	float sine = (float)sin(angle); // ȸ���̵���ȯ ��Ŀ� �� sin��Ÿ ���� ����

	SetGraphicsMode(destDC, GM_ADVANCED); // ������ ��ǥ���� ȸ���� ���Ͽ�, �׷��ȸ�带 Ȯ����� �����մϴ�.(�䱸����:��98,NT�̻�)

	// ȸ���̵���ȯ ����� ����.
	// (�������� API���� ������ 2�� Page(1279) ����.)
	// (���� ���α׷��Ӹ� ���� ���ʼ��а� ���� Page(196) ����.)

	XFORM xform; // �������� ǥ���ϴ� 3��3���� ��� ����
	xform.eM11 = cosine; // 1�� 1�� ���� ���� (ȸ������)
	xform.eM12 = sine; // 1�� 2�� ���� ���� (ȸ������)
	xform.eM21 = -sine; // 2�� 1�� ���� ���� (ȸ������)
	xform.eM22 = cosine; // 2�� 2�� ���� ���� (ȸ������)
	xform.eDx = (FLOAT)dest_width / 2.0f; // 3�� 1�� ���� ���� (X�� �̵� ����)
	xform.eDy = (FLOAT)dest_height / 2.0f; // 3�� 2�� ���� ���� (Y�� �̵� ����)

		// ������ ��ķ�, �μ�DC�� �������� ��ǥ�踦 ��ȯ.
		// �μ�DC�� ��´������ �޸𸮺�Ʈ���� �����Ƿ�,
		// �����ϰ� �ִ� �޸𸮺�Ʈ���� ��ǥü�谡 ȸ���ȴ�.

	SetWorldTransform(destDC, &xform);

	// ȸ���� �޸�DC��, ȸ���� �̹����� ���

	//BitBlt(destDC, -(dest_width / 2), -(dest_height / 2), dest_width, dest_height, sourceDC, source_x, source_y, SRCCOPY);
	StretchBlt(destDC, -(dest_width / 4), -(dest_height / 4), dest_width * 0.5, dest_height * 0.5, sourceDC, source_x, source_y, dest_width, dest_height, SRCCOPY);

	// ���� �ڿ� ����

	SelectObject(sourceDC, hbmOldSource);
	SelectObject(destDC, hbmOldDest);
	DeleteObject(sourceDC);
	DeleteObject(destDC);

	// ȸ���� ��Ʈ�� �ڵ� ����

	return hbmResult;
}

