#define USE_TI89
#define SAVE_SCREEN
#include <tigcclib.h>
#include "extgraph.h"

/*
 * 3D Render engine for Mario Kart 68k
 *
 * by Ben Jones
 * 12/23/2018
 */

unsigned int floor_posx[40][15][16];
unsigned int floor_posy[40][15][16];
char LEVEL[LCD_SIZE];
const int sintable[16] = {0, 98, 181, 237, 256, 237, 181, 98, 0, -98, -181, -237, -256, -237, -181, -98};
const int costable[16] = {256, 237, 181, 98, 0, -98, -181, -237, -256, -237, -181, -98, 0, 98, 181, 237};
char current_angle;
unsigned int current_posx;
unsigned int current_posy;

void draw_track(){
	clrscr();
	FontSetSys(F_8x10);
	DrawStr(50, 50, "I like cats", A_NORMAL);
	FastFillRect(LCD_MEM, 0, 20, 20, 40, A_NORMAL);
	FastFillRect(LCD_MEM, 20, 0, 40, 20, A_NORMAL);
	ngetchx();
	memcpy(LEVEL, LCD_MEM, LCD_SIZE);
}

void precompute(){
	int px;
	int py;
	int x;
	int y;
	unsigned char angle;
	for(px = 0; px < 160; px += 4){
		for(py = 40; py < 100; py += 4){
			for(angle = 0; angle < 16; angle++){
				x = (px - 80)*256/80;
				y = (40 - py)*256/80;
				if(!(y + 20)){
					floor_posx[px/4][py/4 - 10][angle] = 0;
					floor_posy[px/4][py/4 - 10][angle] = 0;
				} else {
					floor_posx[px/4][py/4 - 10][angle] = (((long int) x)*costable[angle] - 200L*sintable[angle])/(y + 20)/80;
					floor_posy[px/4][py/4 - 10][angle] = (((long int) x)*sintable[angle] + 200L*costable[angle])/(y + 20)/80;
				}
			}
		}
	}
}

void render_level(){
	int px;
	int py;
	unsigned int levelx;
	unsigned int levely;
	unsigned char i;
	unsigned char j;
	for(px = 0; px < 160; px += 4){
		for(py = 96; py >= 44; py -= 4){
			levelx = floor_posx[px>>2][(py>>2) - 10][current_angle] + (current_posx>>8);
			levely = floor_posy[px>>2][(py>>2) - 10][current_angle] + (current_posy>>8);
			if(levelx < 160 && levely < 100 && EXT_GETPIX(LEVEL, levelx, levely)){
				for(i = 0; i < 4; i++){
					for(j = 0; j < 4; j++){
						EXT_SETPIX(LCD_MEM, px + i, py + j);
					}
				}
			}
		}
	}
}

void _main(){
	unsigned int key;

	precompute();
	draw_track();
	current_angle = 0;
	current_posx = 45<<8;
	current_posy = 45<<8;
	while(1){
		clrscr();
		render_level();
		key = ngetchx();
		if(key == KEY_LEFT){
			current_angle++;
			if(current_angle == 16){
				current_angle = 0;
			}
		} else if(key == KEY_RIGHT){
			current_angle--;
			if(current_angle == -1){
				current_angle = 15;
			}
		} else if(key == KEY_UP){
			current_posx += sintable[current_angle];
			current_posy -= costable[current_angle];
		} else if(key == KEY_DOWN){
			current_posx -= sintable[current_angle];
			current_posy += costable[current_angle];
		} else if(key == KEY_ESC){
			return;
		}
	}
}
