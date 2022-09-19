#ifndef SCREEN_H
#define SCREEN_H

#define SCR_WIDTH 640
#define SCR_HEIGHT 480
#define FONT_WIDTH 8
#define FONT_HEIGHT 16

#include <SDL2/SDL.h>

void init_screen();

void update();
void render();
void scr_putc(char c);

#endif