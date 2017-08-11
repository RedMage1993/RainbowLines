//*************************************
// Author: Group D (P1)
// Program: BackBuffer.cpp
//
// Description: BackBuffer class source
//
// Revisions:
//  ...
//*************************************

#include "BackBuffer.h"

BackBuffer::BackBuffer(const HWND hWnd, const int width, const int height)
{
	mhWnd = hWnd;

	mWidth = width;
	mHeight = height;

	HDC hWndDC = GetDC(hWnd);

	mhDC = CreateCompatibleDC(hWndDC);

	mhSurface = CreateCompatibleBitmap(hWndDC, width, height);

	ReleaseDC(hWnd, hWndDC);

	mhOldObject = static_cast<HBITMAP> (SelectObject(mhDC, static_cast<HGDIOBJ> (mhSurface)));
}

BackBuffer::~BackBuffer()
{
	SelectObject(mhDC, static_cast<HGDIOBJ> (mhOldObject));
	DeleteObject(static_cast<HGDIOBJ> (mhSurface));
	DeleteDC(mhDC);
}

HDC BackBuffer::getDC()
{
	return mhDC;
}

void BackBuffer::present()
{
	HDC hWndDC = GetDC(mhWnd);

	BitBlt(hWndDC, 0, 0, mWidth, mHeight, mhDC, 0, 0, SRCCOPY);

	ReleaseDC(mhWnd, hWndDC);
}