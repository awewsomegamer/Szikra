#include <screen.h>
#include <emulator.h>
#include <message_handler.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;

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