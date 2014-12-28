#include <stdio.h>
#include "chip8emu.h"

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("Usage: chip8emu <rom file>\n");
		return EXIT_SUCCESS;
	}
	Chip8Emu emu;
	if(Chip8Emu_init(&emu) == EXIT_FAILURE){
		printf("Error initializing Chip8 Emulator\n");
		return EXIT_FAILURE;
	}
	printf("Loading %s\n", argv[1]);
	if(Chip8Emu_load_rom(&emu, argv[1]) == EXIT_FAILURE){
		printf("Error loading file: %s\n", argv[1]);
		return EXIT_FAILURE;
	}
	return Chip8Emu_run(&emu);
}
