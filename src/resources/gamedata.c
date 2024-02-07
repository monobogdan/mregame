#define GAMEDATA_IMPL
#include "gamedata.h"
#include "font.h"

CBitmap sprPlayer, sprBackground, sprEnemy1, sprLaser;

__inline void initSprite(CBitmap* bmp, int w, int h, unsigned char* pixels)
{
	bmp->width = w;
	bmp->height = h;
	bmp->pixels = pixels;
}

void initSpriteData()
{
	initSprite(&sprPlayer, 32, 32, (unsigned char*)&_sprPlayerData);
	initSprite(&sprBackground, 240, 320, (unsigned char*)&_sprBgData);
	initSprite(&sprEnemy1, 32, 32, (unsigned char*)&_sprEnemyData);
	initSprite(&sprLaser, 4, 14, (unsigned char*)&_sprLaser);
}