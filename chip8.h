#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>
#include <stdbool.h>

/* Chip8 constants */
#define CHIP_RAM_SIZE 4096
#define CHIP_STACK_SIZE 16
#define CHIP_NUM_REGS 16
#define CHIP_NUM_KEYS 16
#define CHIP_NUM_X_PIXELS 64
#define CHIP_NUM_Y_PIXELS 32
#define CHIP_NUM_INSTR 16
#define CHIP_VZERO_REG 0
#define CHIP_VF_REG 15

#define CHIP_START_ADDRESS 0x200
#define CHIP_START_STACK 0xF


typedef struct {
	uint8_t memory[CHIP_RAM_SIZE];
	uint8_t registers[CHIP_NUM_REGS];
	uint16_t stack[CHIP_STACK_SIZE];
	uint8_t gfxmemory[CHIP_NUM_X_PIXELS][CHIP_NUM_Y_PIXELS];
	uint8_t key_pad[CHIP_NUM_KEYS];

	uint16_t program_counter;
	uint8_t stack_pointer;
	uint16_t address;
	uint8_t delay_timer;
	uint8_t sound_timer;

	bool update_screen;
	bool wait_key_press;
} Chip8;

Chip8 Chip8_create();
void Chip8_reset(Chip8 *chip);
void Chip8_run_cycle(Chip8 *chip);
void Chip8_draw_sprite(Chip8 *chip, uint8_t x, uint8_t y,uint8_t height);

#endif
