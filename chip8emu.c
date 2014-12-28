#include <stdio.h>
#include "chip8emu.h"

#define DEFAULT_SCREEN_W 640
#define DEFAULT_SCREEN_H 480

#define WHITE 0xFFFFFFFF
#define BLACK 0xFF000000

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
	
	// Create Screen texture
	emu->screen = SDL_CreateTexture(
		emu->ren, 
		SDL_PIXELFORMAT_ARGB8888, 
		SDL_TEXTUREACCESS_STATIC,
		CHIP_NUM_X_PIXELS,
		CHIP_NUM_Y_PIXELS);
	
	memset(emu->pixels, BLACK, sizeof(emu->pixels));
	SDL_UpdateTexture(emu->screen, NULL, emu->pixels, sizeof(uint32_t)*CHIP_NUM_X_PIXELS);
	
	return EXIT_SUCCESS;
}

int Chip8Emu_load_rom(Chip8Emu *emu, const char *filename){
	FILE *fp = NULL;
	fp = fopen(filename, "rb");
	if(fp != NULL){
		fread(emu->chip.memory + CHIP_START_ADDRESS, 
			CHIP_RAM_SIZE - CHIP_START_ADDRESS, 1, fp);
	} else {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int Chip8Emu_run(Chip8Emu *emu){
	bool running = true;
	uint32_t frame_ticks = SDL_GetTicks();
	double frequency = SDL_GetPerformanceFrequency();
	uint32_t cycle_ticks = SDL_GetPerformanceCounter();
	SDL_Event e;
	while(running){
		while(SDL_PollEvent(&e)){
			if(Chip8Emu_handle_event(emu, e) == false){
				running = false;
			}
		}

		// Run Chip8 Cycle
		// TODO fix cycle timing for processor
		if(emu->chip.wait_key_press == false){
			Chip8_run_cycle(&emu->chip);
		}

		// Render screen 16 ms (60 hz)
		// Also decrement timers at 60hz
		if((SDL_GetTicks() - frame_ticks) > 16){
			if(emu->chip.delay_timer > 0)
				emu->chip.delay_timer--;
			if(emu->chip.sound_timer > 0){
				emu->chip.sound_timer--;
				// TODO add buzzer code
			}
			if(emu->chip.update_screen){
				Chip8Emu_update_screen(emu);
				emu->chip.update_screen = false;
			}
			SDL_RenderClear(emu->ren);
			SDL_RenderCopy(emu->ren, emu->screen, NULL, NULL);
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
	}else if(e.type == SDL_KEYDOWN){
		switch(e.key.keysym.sym){
			case SDLK_x:
				emu->chip.key_pad[0x0] = 1;
				break;
			case SDLK_1:
				emu->chip.key_pad[0x1] = 1;
				break;
			case SDLK_2:
				emu->chip.key_pad[0x2] = 1;
				break;
			case SDLK_3:
				emu->chip.key_pad[0x3] = 1;
				break;
			case SDLK_q:
				emu->chip.key_pad[0x4] = 1;
				break;
			case SDLK_w:
				emu->chip.key_pad[0x5] = 1;
				break;
			case SDLK_e:
				emu->chip.key_pad[0x6] = 1;
				break;
			case SDLK_a:
				emu->chip.key_pad[0x7] = 1;
				break;
			case SDLK_s:
				emu->chip.key_pad[0x8] = 1;
				break;
			case SDLK_d:
				emu->chip.key_pad[0x9] = 1;
				break;
			case SDLK_z:
				emu->chip.key_pad[0xA] = 1;
				break;
			case SDLK_c:
				emu->chip.key_pad[0xB] = 1;
				break;
			case SDLK_4:
				emu->chip.key_pad[0xC] = 1;
				break;
			case SDLK_r:
				emu->chip.key_pad[0xD] = 1;
				break;
			case SDLK_f:
				emu->chip.key_pad[0xE] = 1;
				break;
			case SDLK_v:
				emu->chip.key_pad[0xF] = 1;
				break;
		}
		emu->chip.wait_key_press = false;
	} else if(e.type == SDL_KEYUP){
		switch(e.key.keysym.sym){
			case SDLK_x:
				emu->chip.key_pad[0x0] = 0;
				break;
			case SDLK_1:
				emu->chip.key_pad[0x1] = 0;
				break;
			case SDLK_2:
				emu->chip.key_pad[0x2] = 0;
				break;
			case SDLK_3:
				emu->chip.key_pad[0x3] = 0;
				break;
			case SDLK_q:
				emu->chip.key_pad[0x4] = 0;
				break;
			case SDLK_w:
				emu->chip.key_pad[0x5] = 0;
				break;
			case SDLK_e:
				emu->chip.key_pad[0x6] = 0;
				break;
			case SDLK_a:
				emu->chip.key_pad[0x7] = 0;
				break;
			case SDLK_s:
				emu->chip.key_pad[0x8] = 0;
				break;
			case SDLK_d:
				emu->chip.key_pad[0x9] = 0;
				break;
			case SDLK_z:
				emu->chip.key_pad[0xA] = 0;
				break;
			case SDLK_c:
				emu->chip.key_pad[0xB] = 0;
				break;
			case SDLK_4:
				emu->chip.key_pad[0xC] = 0;
				break;
			case SDLK_r:
				emu->chip.key_pad[0xD] = 0;
				break;
			case SDLK_f:
				emu->chip.key_pad[0xE] = 0;
				break;
			case SDLK_v:
				emu->chip.key_pad[0xF] = 0;
				break;
		}
	}
	return true;
}

void Chip8Emu_update_screen(Chip8Emu *emu){
	for(int y=0; y<CHIP_NUM_Y_PIXELS; y++){
		for(int x=0; x<CHIP_NUM_X_PIXELS; x++){
			if(emu->chip.gfxmemory[x][y] == 0)
				emu->pixels[y*CHIP_NUM_X_PIXELS + x] = BLACK;
			else
				emu->pixels[y*CHIP_NUM_X_PIXELS + x] = WHITE;
		}
	}
	SDL_UpdateTexture(emu->screen, NULL, emu->pixels, sizeof(uint32_t)*CHIP_NUM_X_PIXELS);
}
