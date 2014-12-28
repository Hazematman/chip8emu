#include <stdio.h>
#include "chip8emu.h"


int main(int argc, char *argv[]){
	Chip8Emu emu;
	if(Chip8Emu_init(&emu) == EXIT_FAILURE){
		printf("Error initializing Chip8 Emulator\n");
		return EXIT_FAILURE;
	}
	return Chip8Emu_run(&emu);
}
