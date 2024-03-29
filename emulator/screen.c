#include <screen.h>
#include <emulator.h>
#include <interrupts.h>
#include <message_handler.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;
SDL_Event event;

extern uint8_t _FONT;

int cx = 0;
int cy = 0;

uint32_t VRAM[480][640];


void init_screen() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		fatal_error("Failed to initialize SDL: %s", SDL_GetError());

	window = SDL_CreateWindow("Szikra Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_ShowWindow(window);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 0, 640, 480);

	SDL_Delay(100);
}

void update() {
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_KEYDOWN:
		ports[1] = event.key.keysym.scancode;
		call_interrupt(I_INTERRUPT_KEYBOARD);
		break;
		
	case SDL_QUIT:
		emulator_running = 0;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
}

void render() {
	SDL_UpdateTexture(texture, NULL, (void*)&VRAM, 640*4);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_Delay(16);
}

void scr_putp(uint32_t where, uint32_t color) { 
	VRAM[(where) & 0xFFFF][(where >> 16) & 0xFFFF] = color;
}

void scr_putc(char c, uint32_t color) {
	switch (c) {
	case '\n':
		cy += FONT_HEIGHT;

	case '\r':
		cx = 0;

		return;

	case '\b':
		if (cx == 0) {
			cx = SCR_WIDTH - FONT_WIDTH;
			cy -= FONT_HEIGHT;
			scr_putc(' ', color);
			cx = SCR_WIDTH - FONT_WIDTH;
			cy -= FONT_HEIGHT;
		} else {
			cx -= FONT_WIDTH;
			scr_putc(' ', color);
			cx -= FONT_WIDTH;
		}

		return;
	}

	uint8_t* data = &_FONT + c * FONT_HEIGHT;

	int rx = 0;
	for (int i = 0; i < FONT_HEIGHT; i++) {
		for (int j = FONT_WIDTH - 1; j >= 0; j--) {
			if ((data[i] >> j) & 1) {
				VRAM[cy + i][cx + rx] = color;
			} else {
				VRAM[cy + i][cx + rx] = 0;
			}
			SDL_RenderPresent(renderer);
			rx++;
		}

		rx = 0;
	}

	cx += FONT_WIDTH;

	if (cx > SCR_WIDTH - FONT_WIDTH) {
		cy += FONT_HEIGHT;
		cx = 0;
	}
	
	if (cy > SCR_HEIGHT - FONT_HEIGHT) {
		cy = 0;
		cx = 0;
	}
}

