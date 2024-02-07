
#include "../platform.h"

#include "vmsys.h"
#include "vmio.h"
#include "vmgraph.h"
#include "vmchset.h"
#include "vmstdlib.h"

VMINT layers[2];
VMUINT8* screenBuf;
VMINT gTimer;

void handle_sysevt(VMINT message, VMINT param);
void handle_keyevt(VMINT event, VMINT keycode);
void handle_penevt(VMINT event, VMINT x, VMINT y);

int keyState;

/*
* demo
*/
static void draw_hello(void);

/*
* entry
*/
void vm_main(void) {
	layers[0] = -1;
	gameStart();
	
	vm_reg_sysevt_callback(handle_sysevt);
	vm_reg_keyboard_callback(handle_keyevt);
	vm_reg_pen_callback(handle_penevt);
}

void onTimerTick(VMINT tid)
{
	gameUpdate();
	gameDraw();
	vm_graphic_flush_layer(layers, 2);
}

void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
		case VM_MSG_CREATE:
		case VM_MSG_ACTIVE:
			
			layers[0] = vm_graphic_create_layer(0, 0, 
				vm_graphic_get_screen_width(),		
				vm_graphic_get_screen_height(),		
				-1);
			
			vm_graphic_set_clip(0, 0, 
				vm_graphic_get_screen_width(), 
				vm_graphic_get_screen_height());
				
			screenBuf = vm_graphic_get_layer_buffer(layers[0]);
			gTimer = vm_create_timer(16, onTimerTick);
			
			break;
			
		case VM_MSG_PAINT:
			
			break;
			
		case VM_MSG_INACTIVE:
		case VM_MSG_QUIT:
			if( layers[0] != -1 )
				vm_graphic_delete_layer(layers[0]);
			
			vm_delete_timer(gTimer);
			
			break;	
	}
}

void handle_keyevt(VMINT event, VMINT keycode) {
	int vKey = 0;
	
	switch(keycode)
		{
			case VM_KEY_LEFT:
				vKey = KEY_LEFT;
				break;
			case VM_KEY_RIGHT:
				vKey = KEY_RIGHT;
				break;
			case VM_KEY_UP:
				vKey = KEY_UP;
				break;
			case VM_KEY_DOWN:
				vKey = KEY_DOWN;
				break;
			
			case VM_KEY_OK:
				vKey = KEY_OK;
				break;
			case VM_KEY_LEFT_SOFTKEY:
				vKey = KEY_LS;
				break;
			case VM_KEY_RIGHT_SOFTKEY:
				vKey = KEY_RS;
				break;
		}
		
		if(event == VM_KEY_EVENT_DOWN)
			keyState |= vKey;
		
		if(event == VM_KEY_EVENT_UP)
			keyState &= ~vKey;
}

void handle_penevt(VMINT event, VMINT x, VMINT y)
{
	
}

static void draw_hello(void) {
	
	vm_graphic_color color;
	/* flush the screen with text data */
	color.vm_color_565 = VM_COLOR_BLUE;
	vm_graphic_setcolor(&color);
	
	/* fill rect with screen color */
	vm_graphic_fill_rect_ex(layers[0], 0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height());
	
	/* set text color */
	color.vm_color_565 = VM_COLOR_BLUE;
	vm_graphic_setcolor(&color);
	
	/* flush the screen with text data */
	vm_graphic_flush_layer(layers, 2);
}


/* Implementation */

void* sysAlloc(int len)
{
	return vm_malloc(len);
}

void sysFree(void* ptr)
{
	vm_free(ptr);
}

int sysRand()
{
	return 0;
}

int gGetScreenWidth()
{
	return vm_graphic_get_screen_width();
}

int gGetScreenHeight()
{
	return vm_graphic_get_screen_height();
}

int gGetScreenColorDepth()
{
	return 16;
}

void gClearScreen(CColor* color)
{
	vm_graphic_color col;
	col.vm_color_565 = VM_COLOR_888_TO_565(color->r, color->g, color->b);
	
	vm_graphic_setcolor(&col);
	vm_graphic_fill_rect_ex(layers[0], 0, 0, gGetScreenWidth(), gGetScreenHeight());
}


void gPrepareBitmap(CBitmap* bmp)
{
	VMINT cnvs = vm_graphic_create_canvas(bmp->width, bmp->height);
	VMINT layer = vm_graphic_create_layer_ex(0, 0, bmp->width, bmp->height, VM_COLOR_888_TO_565(255, 0, 255), VM_BUF, vm_graphic_get_canvas_buffer(cnvs));
	memcpy(vm_graphic_get_layer_buffer(layer), bmp->pixels, bmp->width * bmp->height * 2);
	
	vm_graphic_canvas_set_trans_color(cnvs, VM_COLOR_888_TO_565(255, 0, 255));
	
	bmp->_platData = (void*)cnvs;
}

void gDrawBitmap(CBitmap* bmp, int x, int y)
{
	int i, j;
	if(!bmp->_platData)
		gPrepareBitmap(bmp);
	
	vm_graphic_blt(screenBuf, x, y, vm_graphic_get_canvas_buffer((VMINT)bmp->_platData), 0, 0, bmp->width, bmp->height, 1);
}

void gDrawBitmapEx(CBitmap* bmp, int x, int y, CColor* colorKey, CColor* mulColor)
{
	
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