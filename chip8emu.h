#ifndef CHIP8EMU_H
#define CHIP8EMU_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "chip8.h"

typedef struct {
	SDL_Window *win;
	SDL_Renderer *ren;
	Chip8 chip;
	SDL_Texture *screen;
	uint32_t pixels[CHIP_NUM_X_PIXELS*CHIP_NUM_Y_PIXELS];
} Chip8Emu;

int Chip8Emu_init(Chip8Emu *emu);
int Chip8Emu_run(Chip8Emu *emu);
bool Chip8Emu_handle_event(Chip8Emu *emu, SDL_Event e);
void Chip8Emu_update_screen(Chip8Emu *emu);

#endif
