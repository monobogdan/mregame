#ifndef __WIN32

#define VERSION "1.0"

#define PLAT_IMPL
#include "../platform.h"
#include <stdlib.h>
#include <stdio.h>
#include <varargs.h>
#include <Windows.h>
#include <gl/GL.h>

void sysLogf(char* fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vprintf(fmt, va);
	va_end(va);
}

void* sysAlloc(int len)
{
	return malloc(len);
}

void sysFree(void* ptr)
{
	free(ptr);
}

int sysRand()
{
	return rand();
}


/* Graphics */

HWND hwnd;

HBITMAP hBackBuf;
HDC hDC;
HDC hPrimaryDC;
HGLRC hGL;

void gInit()
{
	sysLogf("Initializing graphics...\n");

	hwnd = CreateWindowA("STATIC", "2D Framework", WS_VISIBLE | WS_SYSMENU, 0, 0, gGetScreenWidth(), gGetScreenHeight(), 0, 0, 0, 0);
	hPrimaryDC = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	
	if (!SetPixelFormat(hPrimaryDC, ChoosePixelFormat(hPrimaryDC, &pfd), &pfd))
	{
		sysLogf("SetPixelFormat failed\n");
		exit(-1);
	}

	hGL = wglCreateContext(hPrimaryDC);
	wglMakeCurrent(hPrimaryDC, hGL);

	sysLogf("Renderer: %s\n", glGetString(GL_RENDERER));
	sysLogf("Vendor: %s\n", glGetString(GL_VENDOR));
	sysLogf("Version: %s\n", glGetString(GL_VERSION));

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, gGetScreenWidth(), gGetScreenHeight(), 0, 0, 1);
}

int gGetScreenWidth()
{
	return 240;
}

int gGetScreenHeight()
{
	return 320;
}

int gGetScreenColorDepth()
{
	return 16;
}

void gClearScreen(CColor* color)
{
	float r = (float)color->r / 255;
	float g = (float)color->g / 255;
	float b = (float)color->b / 255;

	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define TEXTURE_COLORKEY 63519

void gPrepareBitmap(CBitmap* bmp)
{
	GLuint tex[1];
	glGenTextures(1, &tex);

	sysLogf("Uploading texture %dx%d\n", bmp->width, bmp->height);

	unsigned char* data = (unsigned char*)malloc(bmp->width * bmp->height * 4);

	// Quick endian flip & color-space conversion
	for (int i = 0; i < bmp->width * bmp->height; i++)
	{
		/*unsigned short val = bmp->pixels[i * 2];
		bmp->pixels[i * 2] = bmp->pixels[i * 2 + 1];
		bmp->pixels[i * 2 + 1] = val;
		*/

		unsigned short pixel = *((unsigned short*)&bmp->pixels[i * 2]);

		float r = (float)(pixel & 31) / 32;
		float g = (float)((pixel >> 5) & 63) / 64;
		float b = (float)(pixel >> 11) / 32;

		data[i * 4 + 2] = (unsigned char)(r * 255);
		data[i * 4 + 1] = (unsigned char)(g * 255);
		data[i * 4] = (unsigned char)(b * 255);

		data[i * 4 + 3] = pixel == TEXTURE_COLORKEY ? 0 : 255;
	}

	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp->width, bmp->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	free(data);

	bmp->_platData = tex[0];
}

void gDrawBitmap(CBitmap* bmp, int x, int y)
{
	gDrawBitmapEx(bmp, x, y, 0, 0);
}

void gDrawBitmapEx(CBitmap* bmp, int x, int y, CColor* colorKey, CColor* mulColor)
{
	if (!bmp->_platData)
		gPrepareBitmap(bmp);

	glBindTexture(GL_TEXTURE_2D, (GLuint)bmp->_platData);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2i(x, y);
	glTexCoord2f(1, 0);
	glVertex2i(x + bmp->width, y);
	glTexCoord2f(1, 1);
	glVertex2i(x + bmp->width, y + bmp->height);
	glTexCoord2f(0, 1);
	glVertex2i(x, y + bmp->height);
	glEnd();
}

/* Input */

static int inKeyBinding[] = {
	VK_LEFT, KEY_LEFT,
	'A', KEY_LEFT,
	VK_RIGHT, KEY_RIGHT,
	'D', KEY_RIGHT,
	VK_UP, KEY_UP,
	'W', KEY_UP,
	VK_DOWN, KEY_DOWN,
	'S', KEY_DOWN,
	VK_RETURN, KEY_OK,
	'Q', KEY_LS,
	'E', KEY_RS
};

bool inHasTouchScreen()
{
	return false;
}

int inGetKeyState()
{
	int result = 0;

	for (int i = 0; i < (sizeof(inKeyBinding) / sizeof(int)) / 2; i++)
	{
		if (GetAsyncKeyState(inKeyBinding[i * 2]) & 0x8000)
			result |= inKeyBinding[i * 2 + 1];
	}

	return result;
}

bool inIsAnyKeyPressed()
{
	return inGetKeyState() != 0;
}

int inGetPointerX()
{
	return 0;
}

int inGetPointerY()
{
	return 0;
}

/* Main */

void winMainLoop()
{
	while (IsWindow(hwnd))
	{
		MSG msg;

		while (PeekMessageA(&msg, hwnd, 0, 0, PM_REMOVE))
			DefWindowProc(hwnd, msg.message, msg.wParam, msg.lParam);

		gameUpdate();
		gameDraw();

		glFinish();
		SwapBuffers(hPrimaryDC);

		Sleep(1000 / 60);
	}
}

int main(int argc, char** argv)
{
	sysLogf("Portable 2D framework\n");
	sysLogf("Version: " VERSION "\n");
	
	gInit();

	gameStart();
	winMainLoop();
}

#endif