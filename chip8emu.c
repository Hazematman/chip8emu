#include "chip8emu.h"

#define DEFAULT_SCREEN_W 640
#define DEFAULT_SCREEN_H 480

int Chip8Emu_init(Chip8Emu *emu){
	// Init everything
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	
	// Create the window and renderer
	emu->win = SDL_CreateWindow(
			"Chip8 Emulator", // Window title
			SDL_WINDOWPOS_UNDEFINED, // Window X position
			SDL_WINDOWPOS_UNDEFINED, // Window Y position
			DEFAULT_SCREEN_W, // Window width
			DEFAULT_SCREEN_H, // Window height
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE); // Window flags
	if(emu->win == NULL){
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		SDL_Quit();
		return EXIT_FAILURE;
	}

	emu->ren = SDL_CreateRenderer(emu->win, 1, SDL_RENDERER_ACCELERATED);
	if(emu->ren == NULL){
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
		return EXIT_FAILURE;
	}

	// Init Chip8
	emu->chip = Chip8_create();
	return EXIT_SUCCESS;
}

int Chip8Emu_run(Chip8Emu *emu){
	bool running = true;
	uint32_t frame_ticks = SDL_GetTicks();
	SDL_Event e;
	while(running){
		while(SDL_PollEvent(&e)){
			if(Chip8Emu_handle_event(emu, e) == false){
				running = false;
			}
		}

		// Run Chip8 Cycle
		Chip8_run_cycle(&emu->chip);

		// Render screen 16 ms (60 hz)
		// Also decrement timers at 60hz
		if((SDL_GetTicks() - frame_ticks) > 16){
			if(emu->chip.delay_timer > 0)
				emu->chip.delay_timer--;
			if(emu->chip.sound_timer > 0){
				emu->chip.sound_timer--;
				// TODO add buzzer code
			}
			SDL_RenderClear(emu->ren);
			// TODO convert gfxmemory to image
			SDL_RenderPresent(emu->ren);
			
			// Reset frame ticks
			frame_ticks = SDL_GetTicks();
		}
	}
	return EXIT_SUCCESS;
}

bool Chip8Emu_handle_event(Chip8Emu *emu, SDL_Event e){
	if(e.type == SDL_QUIT){
		return false;
	}
	return true;
}
