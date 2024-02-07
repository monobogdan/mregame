#include "platform.h"

#include "resources/font.h"
/*
unsigned char* screenBuffer;

#define RGB888_TO_565(r, g, b) (unsigned short) ( ((unsigned int)(r>>3) << 11) + ((unsigned int)(g>>2) << 5) + ((unsigned int)(b>>3)) )

void* gInit(int width, int height)
{
	screenBuffer = (unsigned char*)sysAlloc(width * height * 2);

	return screenBuffer;
}

void gClearScreen(CColor* color)
{
	int i;
	unsigned short col = RGB888_TO_565(color->r, color->g, color->b);

	for (i = 0; i < gGetScreenWidth() * gGetScreenHeight(); i++)
	{
		*((unsigned short*)&screenBuffer[i * 2]) = col;
	}

	*((unsigned short*)&screenBuffer[(35 * 240 + 2) * 2]) = RGB888_TO_565(255, 0, 0);
}

__inline unsigned short swapEndianess(unsigned short color)
{
	unsigned char* pixels = (unsigned char*)&color;
	unsigned char p1 = pixels[0];

	pixels[0] = pixels[1];
	pixels[1] = p1;

	return *((unsigned short*)pixels);
}

void gDrawBitmap(CBitmap* bmp, int x, int y)
{
	if (bmp)
	{
		int i, j;

		for (i = 0; i < bmp->height; i++)
		{
			if (i + y >= gGetScreenHeight())
				break;

			for (j = 0; j < bmp->width; j++)
			{
				unsigned short srcPixel;

				if (j + x >= gGetScreenWidth())
					break;

				srcPixel = *((unsigned short*)&bmp->pixels[(i * bmp->width + j) * 2]);
				*((unsigned short*)&screenBuffer[((i + y) * gGetScreenWidth() + (j + x)) * 2]) = swapEndianess(srcPixel);
			}
		}
	}
}

void gDrawText(char* text, int x, int y, CColor* color)
{
	if (text && strlen(text) > 0)
	{

	}
}*/