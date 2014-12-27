#include <string.h>
#include "chip8.h"

uint8_t chip_font_data[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

Chip8 Chip8_create(){
	Chip8 chip;
	Chip8_reset(&chip);
	return chip;
}

void Chip8_reset(Chip8 *chip){
	// Set all of chips arrays to zero
	memset(chip->memory,0,CHIP_RAM_SIZE);
	memset(chip->registers, 0, CHIP_NUM_REGS);
	memset(chip->stack, 0, CHIP_STACK_SIZE*sizeof(uint16_t));
	memset(chip->gfxmemory, 0, CHIP_NUM_X_PIXELS*CHIP_NUM_Y_PIXELS);
	memset(chip->key_pad, 0, CHIP_NUM_KEYS);

	// Copy font data to chip memory
	memcpy(chip->memory, chip_font_data, sizeof(chip_font_data));

	// Set default values of important registers
	chip->program_counter = CHIP_START_ADDRESS;
	chip->stack_pointer = CHIP_START_STACK;
	chip->address = 0x0000;
	chip->delay_timer = 0x00;
	chip->sound_timer = 0x00;

	chip->update_screen = false;
}
