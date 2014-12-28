#ifndef CHIP8EMU_H
#define CHIP8EMU_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "chip8.h"

typedef struct {
	SDL_Window *win;
	SDL_Renderer *ren;
	Chip8 chip;
} Chip8Emu;

int Chip8Emu_init(Chip8Emu *emu);
int Chip8Emu_run(Chip8Emu *emu);
bool Chip8Emu_handle_event(Chip8Emu *emu, SDL_Event e);

#endif
