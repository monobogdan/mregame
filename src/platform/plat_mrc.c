
#include "../platform.h"

/* MRP event handlers */

#include <mr_helper.h>

mr_screeninfo screenInfo;
int keyState;
int globalTimer;

extern unsigned char _sprPlayerData[];

void mrc_draw(int32 data)
{
	mrc_clearScreen(0, 0, 128);
	
	gameUpdate();
	gameDraw();
	
	mrc_refreshScreen(0, 0, 240, 320);
}

int32 mrc_init(void)
{
	mrc_getScreenInfo(&screenInfo);

	gameStart();
	
	// Allocate timer
	globalTimer = mrc_timerCreate();
	mrc_timerStart(globalTimer, 1000 / 60, 0, mrc_draw, 1);
	
	return MR_SUCCESS;
}

int32 mrc_event(int32 ev, int32 p0, int32 p1)
{
	int key = p0;
	int vKey = 0;
	
	switch(key)
		{
			case MR_KEY_LEFT:
				vKey = KEY_LEFT;
				break;
			case MR_KEY_RIGHT:
				vKey = KEY_RIGHT;
				break;
			case MR_KEY_UP:
				vKey = KEY_UP;
				break;
			case MR_KEY_DOWN:
				vKey = KEY_DOWN;
				break;
			
			case MR_KEY_SELECT:
				vKey = KEY_OK;
				break;
			case MR_KEY_SOFTLEFT:
				vKey = KEY_LS;
				break;
			case MR_KEY_SOFTRIGHT:
				vKey = KEY_RS;
				break;
		}
		
		if(ev == MR_KEY_PRESS)
			keyState |= vKey;
		
		if(ev == MR_KEY_RELEASE)
			keyState &= ~vKey;
	
	return MR_SUCCESS;

}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
return MR_SUCCESS;
}
int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4,int32 p5)
{
return MR_SUCCESS;
}


int32 mrc_pause(void)
{
	return MR_SUCCESS;
}


int32 mrc_resume(void)
{
	return MR_SUCCESS;
}


int32 mrc_exitApp(void)
{
	
	return MR_SUCCESS;
}

/* Implementation */

void* sysAlloc(int len)
{
	return mrc_malloc(len);
}

void sysFree(void* ptr)
{
	mrc_free(ptr);
}

int sysRand()
{
	mrc_sand(mrc_getUptime());
	return mrc_rand();
}

int gGetScreenWidth()
{
	return screenInfo.width;
}

int gGetScreenHeight()
{
	return screenInfo.height;
}

int gGetScreenColorDepth()
{
	return screenInfo.bit;
}

void gClearScreen(CColor* color)
{
	mrc_clearScreen(color->r, color->g, color->b);
}

void gDrawBitmap(CBitmap* bmp, int x, int y)
{
	mrc_bitmapShowEx((uint16*)bmp->pixels, x, y, bmp->width, bmp->width, bmp->height, BM_TRANSPARENT, 0, 0);
}

void gDrawBitmapEx(CBitmap* bmp, int x, int y, CColor* colorKey, CColor* mulColor)
{
	gDrawBitmap(bmp, x, y);
	// Not supported yet on MRP
}

void gDrawText(char* text, int x, int y, CColor* color)
{

}

bool inHasTouchScreen()
{
	return 0;
}

int inGetKeyState()
{
	return keyState;
}

bool inIsAnyKeyPressed()
{
	return keyState > 0;
}

int inGetPointerX()
{
	return 0;
}

int inGetPointerY()
{
	return 0;
}