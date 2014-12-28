#include <stdlib.h>
#include <string.h>
#include "chip8.h"

#define MASK(x,y) ((x) & (y))
#define REGX(chip,op) chip->registers[MASK(op,0x0F00)]
#define REGY(chip,op) chip->registers[MASK(op,0x00F0)]
#define REG0(chip) chip->registers[CHIP_VZERO_REG]
#define REGV(chip) chip->registers[CHIP_VF_REG]
#define INDEX(op) MASK(op, 0x0FFF)
#define IMMEDIATE(op) MASK(op,0x00FF)
#define NIB(op) MASK(op, 0x000F)
#define WRAPGFX(chip,x,y) chip->gfxmemory[x % CHIP_NUM_X_PIXELS][y % CHIP_NUM_Y_PIXELS]

/* More constants for chip8 */
#define CHIP_FONT_HEIGHT 5

const uint8_t chip_font_data[] = {
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

void (*chip_instructions[CHIP_NUM_INSTR]) (Chip8 *chip, uint16_t opcode);
void chip_instr0(Chip8 *chip, uint16_t opcode);
void chip_instr1(Chip8 *chip, uint16_t opcode);
void chip_instr2(Chip8 *chip, uint16_t opcode);
void chip_instr3(Chip8 *chip, uint16_t opcode);
void chip_instr4(Chip8 *chip, uint16_t opcode);
void chip_instr5(Chip8 *chip, uint16_t opcode);
void chip_instr6(Chip8 *chip, uint16_t opcode);
void chip_instr7(Chip8 *chip, uint16_t opcode);
void chip_instr8(Chip8 *chip, uint16_t opcode);
void chip_instr9(Chip8 *chip, uint16_t opcode);
void chip_instrA(Chip8 *chip, uint16_t opcode);
void chip_instrB(Chip8 *chip, uint16_t opcode);
void chip_instrC(Chip8 *chip, uint16_t opcode);
void chip_instrD(Chip8 *chip, uint16_t opcode);
void chip_instrE(Chip8 *chip, uint16_t opcode);
void chip_instrF(Chip8 *chip, uint16_t opcode);

Chip8 Chip8_create(){
	Chip8 chip;
	Chip8_reset(&chip);

	// Set all chip instructions
	chip_instructions[0x0] = chip_instr0;
	chip_instructions[0x1] = chip_instr1;
	chip_instructions[0x2] = chip_instr2;
	chip_instructions[0x3] = chip_instr3;
	chip_instructions[0x4] = chip_instr4;
	chip_instructions[0x5] = chip_instr5;
	chip_instructions[0x6] = chip_instr6;
	chip_instructions[0x7] = chip_instr7;
	chip_instructions[0x8] = chip_instr8;
	chip_instructions[0x9] = chip_instr9;
	chip_instructions[0xA] = chip_instrA;
	chip_instructions[0xB] = chip_instrB;
	chip_instructions[0xC] = chip_instrC;
	chip_instructions[0xD] = chip_instrD;
	chip_instructions[0xE] = chip_instrE;
	chip_instructions[0xF] = chip_instrF;
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

	chip->update_screen = true;
}

void Chip8_run_cycle(Chip8 *chip){
	// Get opcode and increment program counter
	uint16_t opcode = chip->memory[chip->program_counter] << 8 | 
						chip->memory[chip->program_counter+1];
	chip->program_counter = MASK(chip->program_counter+2, 0xFFF);
	
	// Call the correct instruction from opcode
	chip_instructions[(uint8_t) MASK(opcode, 0xF000)](chip, opcode);
}

void Chip8_draw_sprite(Chip8 *chip, uint8_t x, uint8_t y, uint8_t height){
	// Set collision flag to 0
	REGV(chip) = 0;

	// Draw sprite
	for(uint8_t ypos=0; ypos<height; ypos++){
		uint8_t line = chip->memory[MASK(chip->address+ypos, 0xFFF)];
		for(uint8_t xpos=0; xpos<8; xpos++){ // Check pixels 1 by 1
			// Check if pixel at xpos is white or black
			if((line & (0x80 >> xpos)) != 0){
				// Check for pixel collision
				if(WRAPGFX(chip, x+xpos, y+ypos) == 1)
					REGV(chip) = 1;

				WRAPGFX(chip, x+xpos, y+ypos) ^= 1; // Blit pixel
			}
		}
	}
	chip->update_screen = true;
}

void chip_instr0(Chip8 *chip, uint16_t opcode){
	if(opcode == 0xE0){ // Clear the screen
		memset(chip->gfxmemory, 0, CHIP_NUM_X_PIXELS*CHIP_NUM_Y_PIXELS);
		chip->update_screen = true;
	} else if(opcode == 0xEE){ // Return from subroutine
		chip->program_counter = chip->stack[chip->stack_pointer];
		chip->stack_pointer = MASK(chip->stack_pointer-1, 0xFF);
	}
}
void chip_instr1(Chip8 *chip, uint16_t opcode){
	// Set program counter to address mask
	chip->program_counter = MASK(opcode, 0xFFF);
}
void chip_instr2(Chip8 *chip, uint16_t opcode){
	// Call subroutine
	chip->stack_pointer = MASK(chip->stack_pointer+1,0xF);
	chip->stack[chip->stack_pointer] = chip->program_counter;
	chip->program_counter = MASK(opcode, 0xFFF);
}
void chip_instr3(Chip8 *chip, uint16_t opcode){
	// Skip if Reg[X] == NN
	if(REGX(chip,opcode) == IMMEDIATE(opcode)){
		chip->program_counter += 2;
	}
}
void chip_instr4(Chip8 *chip, uint16_t opcode){
	// Skip if VX != NN
	if(REGX(chip,opcode) != IMMEDIATE(opcode)){
		chip->program_counter += 2;
	}
}
void chip_instr5(Chip8 *chip, uint16_t opcode){
	// Skip if VX == VY
	if(REGX(chip, opcode) == REGY(chip, opcode)){
		chip->program_counter += 2;
	}
}
void chip_instr6(Chip8 *chip, uint16_t opcode){
	// Set VX to immediate value
	REGX(chip, opcode) = IMMEDIATE(opcode);
}
void chip_instr7(Chip8 *chip, uint16_t opcode){
	// Add immediate to VX
	REGX(chip, opcode) += IMMEDIATE(opcode);
}
void chip_instr8(Chip8 *chip, uint16_t opcode){
	switch(NIB(opcode)){
		case 0x1: // OR VX and VY
			REGX(chip, opcode) |= REGY(chip, opcode);
			break;
		case 0x2: // AND VX and VY
			REGX(chip, opcode) &= REGY(chip, opcode);
			break;
		case 0x3: // XOR VX and VY
			REGX(chip, opcode) ^= REGY(chip, opcode);
			break;
		case 0x4: // ADD VX and VY + set carry
			REGV(chip) = (REGX(chip, opcode) - REGY(chip, opcode)) < REGX(chip, opcode);
			REGX(chip, opcode) += REGY(chip, opcode);
			break;
		case 0x5: // SUB VX and VY + set carry
			REGV(chip) = (REGX(chip, opcode) - REGY(chip, opcode)) > REGX(chip, opcode);
			REGX(chip, opcode) -= REGY(chip, opcode);
			break;
		case 0x6: // SHIFT VX right + set VF to first bit
			REGV(chip) = MASK(REGX(chip, opcode), 0x1);
			REGX(chip, opcode) >>= 1;
			break;
		case 0x7: // SUB VY and VX
			REGV(chip) = (REGY(chip, opcode) - REGX(chip, opcode)) < REGY(chip, opcode);
			REGX(chip, opcode) = REGY(chip, opcode) - REGX(chip, opcode);
			break;
		case 0xE: // SHIFT VX left + set VF to last bit
			REGV(chip) = MASK(REGX(chip, opcode), 0x80);
			break;

	}
}
void chip_instr9(Chip8 *chip, uint16_t opcode){
	// Skip if VX != VY
	if(REGX(chip, opcode) != REGY(chip, opcode)){
		chip->program_counter += 2;
	}
}
void chip_instrA(Chip8 *chip, uint16_t opcode){
	// Set address to index
	chip->address = INDEX(opcode);
}
void chip_instrB(Chip8 *chip, uint16_t opcode){
	// Jump to index + VO
	chip->program_counter = MASK(INDEX(opcode) + REG0(chip), 0xFFF);
}
void chip_instrC(Chip8 *chip, uint16_t opcode){
	// Random number with bit mask
	REGX(chip, opcode) = MASK(rand(), IMMEDIATE(opcode));	
}
void chip_instrD(Chip8 *chip, uint16_t opcode){
	// Draw sprite
	Chip8_draw_sprite(chip, REGX(chip, opcode), REGY(chip, opcode), MASK(opcode, 0xF));
}
void chip_instrE(Chip8 *chip, uint16_t opcode){
	// Handle key input
	// First skips if key is down
	// Second skips if key is not down
	if((INDEX(opcode) == 0x9E && (chip->key_pad[REGX(chip, opcode)] == true)) ||
		(INDEX(opcode) == 0xA1 && (chip->key_pad[REGX(chip, opcode)] == false))){

		chip->program_counter += 2;
	}
}
void chip_instrF(Chip8 *chip, uint16_t opcode){
	switch(IMMEDIATE(opcode)){
		case 0x07: // Set VX to delay_timer
			REGX(chip, opcode) = chip->delay_timer;
			break;
		case 0x0A: // Wait for keypress
			// TODO code to wait for keypress
			break;
		case 0x15: // Set delay timer to VX
			chip->delay_timer = REGX(chip, opcode);
			break;
		case 0x18: // Set sound timer to VX
			chip->sound_timer = REGX(chip, opcode);
			break;
		case 0x1E: // ADD VX to address
			chip->address = MASK(chip->address + REGX(chip, opcode), 0xFFF);
			break;
		case 0x29: // Set address to the spirte address of VX
			chip->address = REGX(chip, opcode) * 5;
			break;
		case 0x33: // Convert VX to BCD and store at I,I+1,I+2
			chip->memory[MASK(chip->address, 0xFFF)] = (REGX(chip, opcode) / 100) % 10;
			chip->memory[MASK(chip->address+1, 0xFFF)] = (REGX(chip, opcode) / 10) % 10;
			chip->memory[MASK(chip->address+2, 0xFFF)] = REGX(chip, opcode) % 10;
			break;
		case 0x55: // Set memory of V0 to VX
			for(uint8_t i=0; i < MASK(opcode,0x0F00); i++){
				chip->memory[MASK(chip->address+i, 0xFFF)] = chip->registers[i];
			}
			break;
		case 0x65: // Get memory of V0 to VX
			for(uint8_t i=0; i < MASK(opcode,0x0F00); i++){
				chip->registers[i] = chip->memory[MASK(chip->address+i, 0xFFF)];
			}
			break;
	}
}
