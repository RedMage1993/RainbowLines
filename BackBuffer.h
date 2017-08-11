//*************************************
// Author: Group D (P1)
// Program: BackBuffer.h
//
// Description: BackBuffer class header
//
// Revisions:
//  ...
//*************************************

#ifndef BACKBUFFER_H
#define BACKBUFFER_H

#include <Windows.h>

class BackBuffer
{
public:
	BackBuffer(const HWND hWnd, const int width, const int height);
	~BackBuffer();

	HDC getDC();

	void present();

private:
	HWND mhWnd;
	HDC mhDC;
	HBITMAP mhSurface;
	HBITMAP mhOldObject;
	int mWidth;
	int mHeight;
};

#endif // BACKBUFFER_H