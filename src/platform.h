#ifndef PLATFORM_H
#define PLATFORM_H

#include <string.h>

#ifdef __WIN32
#define PLATFORM "Windows"
#endif

#ifdef _MTK_MRC
#define PLATFORM "MRP"
#endif

#ifdef _MTK_VXP
#define PLATFORM "MTK"
#endif

typedef unsigned char bool;
#define true 1
#define false 0

// Assuming native pixelformat for this platform. Mostly, it's RGB565.
typedef struct {
	unsigned char* pixels;
	int width;
	int height;

	void* _platData; // Used for OpenGL textures on Win32
} CBitmap;

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} CColor;

#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_UP 4
#define KEY_DOWN 8
#define KEY_OK 16
#define KEY_LS 32
#define KEY_RS 64

void sysLogf(char* fmt, ...);
void* sysAlloc(int len);
void sysFree(void* ptr);
int sysRand();

#define RAND_MAX 32768

int gGetScreenWidth();
int gGetScreenHeight();
int gGetScreenColorDepth(); // Almost always 16
void gClearScreen(CColor* color);
void gDrawBitmap(CBitmap* bmp, int x, int y);
void gDrawBitmapEx(CBitmap* bmp, int x, int y, CColor* colorKey, CColor* mulColor);
void gDrawText(char* text, int x, int y, CColor* color);

bool inHasTouchScreen();
int inGetKeyState();
bool inIsAnyKeyPressed();
int inGetPointerX();
int inGetPointerY();

void gameStart();
void gameUpdate();
void gameDraw();

#endif