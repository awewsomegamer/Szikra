#include <screen.h>
#include <emulator.h>
#include <message_handler.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;

extern uint8_t _FONT;

int cx = 0;
int cy = 0;

void init_screen() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		fatal_error("Failed to initialize SDL: %s", SDL_GetError());

	window = SDL_CreateWindow("Szikra Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_ShowWindow(window);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
}

void update() {
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_QUIT:
		emulator_running = 0;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
}

void render() {
	SDL_RenderPresent(renderer);
}

void scr_putc(char c) {
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
			scr_putc(' ');
			cx = SCR_WIDTH - FONT_WIDTH;
			cy -= FONT_HEIGHT;
		} else {
			cx -= FONT_WIDTH;
			scr_putc(' ');
			cx -= FONT_WIDTH;
		}

		return;
	}

	uint8_t* data = &_FONT + c * FONT_HEIGHT;

	int rx = 0;
	for (int i = 0; i < FONT_HEIGHT; i++) {
		for (int j = FONT_WIDTH - 1; j >= 0; j--) {
			if ((data[i] >> j) & 1) {
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderDrawPoint(renderer, (cx + rx), (cy + i));
			}
			rx++;
		}

		rx = 0;
	}

	cx += FONT_WIDTH;
	if (cx > SCR_WIDTH) {
		cx = 0;
		cy += FONT_HEIGHT;
	} else if (cy > SCR_HEIGHT) {
		cy = 0;
	}

}

