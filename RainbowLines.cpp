// include files
#include <Windows.h>
#include <CommCtrl.h>
#include "BackBuffer.h"

// function prototypes
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LOGPEN SetPen(const COLORREF& color = RGB(0, 0, 0), const int width = 1, const UINT style = PS_SOLID);
void MakePoint(const HDC hdc, const int x, const int y, const COLORREF& color = RGB(0, 0, 0));
void AdjustColor(int& red, int& green, int& blue);

// global variables
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct LINE
{
	POINT p0; // starting position
	POINT p1; // ending position
	COLORREF c;
} line;

BackBuffer* bBuffer;
HDC hBackBufferDC;

bool lButtonIsDown;
bool isDrawing;
int red;
int green;
int blue;

DEVMODE dmScreen;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wcx;
	HWND hWnd;
	MSG msg;

	// initialize all members of wcx to zero
    ZeroMemory(reinterpret_cast<PVOID> (&wcx), sizeof(wcx));

	// fill the wcx structure
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.hbrBackground = static_cast<HBRUSH> (GetStockObject(NULL_BRUSH)); // GetSysColorBrush(COLOR_WINDOW);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcx.hInstance = hInstance;
	wcx.lpfnWndProc = WndProc;
	wcx.lpszClassName = "GDIPrctc";
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	
	
		screen.dmSize = sizeof(DEVMODE);
		screen.dmBitsPerPel = 16;
		screen.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		screen.dmPelsWidth = 800;
		screen.dmPelsHeight = 600S;
	
	// register class within call to CreateWindow
	hWnd = CreateWindowEx(NULL, MAKEINTRESOURCE(RegisterClassEx(&wcx)), "GDI Practice",
		WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL, NULL, hInstance, NULL);
	
	// message retrieval-and-dispatch loop
	while (GetMessage(&msg, NULL, NULL, NULL) > 0) // can return 1, 0, and -1
	{
        TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HPEN hPen;
	HPEN hOldPen;
	PAINTSTRUCT ps;

	switch (uMsg)
	{
	case WM_CREATE:
		bBuffer = new BackBuffer(hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);
		hBackBufferDC = bBuffer->getDC();

		red = 0;
		green = 0;
		blue = 0;

		lButtonIsDown = false;
		isDrawing = false;
		break;
	case WM_PAINT:
		BeginPaint(hwnd, &ps);

		if (isDrawing)
		{
			hPen = CreatePenIndirect(&SetPen(line.c));
			hOldPen = static_cast<HPEN> (SelectObject(hBackBufferDC, static_cast<HGDIOBJ> (hPen)));

			MoveToEx(hBackBufferDC, line.p0.x, line.p0.y, 0);
			LineTo(hBackBufferDC, line.p1.x, line.p1.y);
			MakePoint(hBackBufferDC, line.p1.x, line.p1.y, RGB(255, 255, 255));

			SelectObject(hBackBufferDC, static_cast<HGDIOBJ> (hOldPen));
			DeleteObject(static_cast<HGDIOBJ> (hPen));

			isDrawing = false;
		}

		bBuffer->present();
		EndPaint(hwnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		SetCapture(hwnd);
		lButtonIsDown = true;

		line.p0.x = LOWORD(lParam);
		line.p0.y = HIWORD(lParam);
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		lButtonIsDown = false;
		break;
	case WM_MOUSEMOVE:
		if (lButtonIsDown)
		{
			line.p1.x = static_cast<short> (LOWORD(lParam));
			line.p1.y = static_cast<short> (HIWORD(lParam));

			line.c = RGB(red, green, blue);
			AdjustColor(red, green, blue);

			isDrawing = true;
			InvalidateRect(hwnd, 0, false);
		}

		break;
	case WM_RBUTTONDOWN:
		delete bBuffer;
		bBuffer = 0;

		bBuffer = new BackBuffer(hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);
		hBackBufferDC = bBuffer->getDC();

		InvalidateRect(hwnd, 0, true);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		delete bBuffer;
		bBuffer = 0;

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

LOGPEN SetPen(const COLORREF& color, const int width, const UINT style)
{
	LOGPEN lp;

	lp.lopnColor = color;
	lp.lopnStyle = style;
	lp.lopnWidth.x = width;
	lp.lopnWidth.y = width;

	return lp;
}

void MakePoint(const HDC hdc, const int x, const int y, const COLORREF& color)
{
	SetPixel(hdc, x, y, color); // origin
	SetPixel(hdc, x + 1, y + 1, color); // bottom right
	SetPixel(hdc, x - 1, y - 1, color); // upper left
	SetPixel(hdc, x + 1, y, color); // right
	SetPixel(hdc, x, y + 1, color); // down
	SetPixel(hdc, x - 1, y, color); // left
	SetPixel(hdc, x, y - 1, color); // up
	SetPixel(hdc, x - 1, y + 1, color); // bottom left
	SetPixel(hdc, x + 1, y - 1, color); // upper right
}

void AdjustColor(int& red, int& green, int& blue)
{
	static bool inReverse = false;

	if ((red < 255 || green < 255 || blue < 255) && !inReverse)
	{
		if (green < 255 || blue < 255)
		{
			if (blue < 255)
				blue += 51;
			else
			{
				blue = 0;

				if (green < 255)
					green += 51;
			}
		}
		else
		{
			blue = 0;
			green = 0;

			if (red < 255)
				red += 51;
		}
	}
	else
	{
		if (!inReverse)
		{
			inReverse = true;
			return;
		}
	}

	if ((red > 0 || green > 0 || blue > 0) && inReverse)
	{
		if (green > 0 || blue > 0)
		{
			if (blue > 0)
				blue -= 51;
			else
			{
				blue = 255;

				if (green > 0)
					green -= 51;
			}
		}
		else
		{
			blue = 255;
			green = 255;

			if (red > 0)
				red -= 51;
		}
	}
	else
	{
		if (inReverse)
		{
			inReverse = false;
			return;
		}
	}
}
