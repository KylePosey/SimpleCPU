#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

const uint16_t STACK_START_ADDR = 0x3E81;

uint8_t registers[3] = {};
uint8_t memory[0xFFFF] = {};
/*
	Memory map
	-----------
	0x0000 - 0xFFFF | Full range of addresable ram. 64k
	...................................................
	0x0000 - 0x3E80 | Rom load section. 16k
	0x3E81 - 0x4268 | Stack. 1k
	0x4269 - 0xFFFF | Free ram. 46k
*/

uint16_t pc = 0;
uint8_t sp = 0;

void init_cpu(FILE *fp)
{
	sp = STACK_START_ADDR;

	uint8_t buffer[0x3E80] = {};

	for (int i = 0; i < 0x3E80; i++)
	{
		fread(buffer+i, 1, 1, fp);
	}

	for (int j = 0; j < 0x3E80; j++)
	{
		memory[j] = buffer[j];
	}
}

uint16_t combine_bytes(uint8_t b1, uint8_t b2)
{
	return (((uint16_t) b1 << 8) | b2);
}
/*
uint8_t* split_bytes(uint16_t word)
{
	static const uint8_t hi_lo[] = { (uint8_t)(value >> 8), (uint8_t)value };
	return &hi_lo;
}*/

uint8_t read_byte()
{
	return memory[pc];
}


//OP Codes
//0- Instructions
void OP0A_NOP()
{
	//NOOPERATION
	pc++;
}
void OP0B_JMP()
{
	//jump to rom address for further execution
	pc++;
	uint8_t b1 = read_byte();
	pc++;
	uint8_t b2 = read_byte();
	uint16_t word = combine_bytes(b1, b2);
	if (word < 0x0000 && word > 0x3E80)
	{
		printf("FATAL ERROR: Segmentation Fault\n");
		exit(139);
	}
	else
	{
		pc = word;
		printf("Jumped to %02hhX, containing, %02hhX\n", pc, memory[pc]);
	}

}
//1- Instructions
void OP1A_RSA()
{
	//set a register to 0
	registers[0] = 0x000;
	printf("Register a: %02hhX\n", registers[0]);
	pc++;
}
void OP1B_LDA()
{
	//Load value into accumulator
	pc++;
	uint8_t ins = read_byte();
	if (ins == 0x00)
	{
		pc++;
		uint8_t b1 = read_byte();
		pc++;
		registers[0] = b1;
		printf("register: %02hhX | real: %02hhX\n", registers[0], b1);
	}
	else
	{
		uint8_t b1 = read_byte();
		pc++;
		uint8_t b2 = read_byte();
		pc++;
		uint16_t word = combine_bytes(b1, b2);
		if (word < 0x4269 || word > 0xFFFF)
		{
			printf("FATAL ERROR: Segmentation Fault\n");
			exit(139);
		}
		registers[0] = memory[word];
		printf("register: %02hhX | %02hhX: %02hhX\n", registers[0], word, memory[word]);
	}
}
void OP1C_STA()
{
	//store a register to memory address
	pc++;
	uint8_t b1 = read_byte();
	pc++;
	uint8_t b2 = read_byte();
	pc++;
	uint16_t word = combine_bytes(b1, b2);
	if (word < 0x4269 || word > 0xFFFF)
	{
		printf("FATAL ERROR: Segmentation Fault\n");
		exit(139);
	}
	memory[word] = registers[0];
}
//2- Instructions
void OP2A_PHC()
{
	//Push PC to stack
	memory[sp] = pc;
	sp++;
	pc++;
	printf("pushed %02hhX to stack at location %02hhX \n", pc-1, sp-1);
}
void OP2B_PPC()
{
	//Pop stack to PC
	sp--;
	pc = memory[sp];
	printf("popped %02hhX to stack from location %02hhX \n", pc, sp+1);
	//pc++;
}
void OP2C_PHA()
{
	//Push A to stack
	memory[sp] = registers[0];
	sp++;
	pc++;
	printf("pushed %02hhX to stack at location %02hhX \n", registers[0], sp-1);
}
void OP2D_PPA()
{
	//Pop stack to A
	sp--;
	registers[0] = memory[sp];
	printf("popped %02hhX to stack from location %02hhX \n", registers[0], sp);
	pc++;
}
//3- Instructions
void OP3A_ADD()
{
	//add to a register
	printf("register before: %02hhX | ", registers[0]);
	pc++;
	uint8_t ins = read_byte();
	if (ins == 0x00)
	{
		pc++;
		uint8_t b1 = read_byte();
		pc++;
		if ((registers[0] + b1) > 0xFF)
		{
			registers[0] = 0xFF;
		}
		else
		{
			registers[0] += b1;
		}
	}
	else
	{
		uint8_t b1 = read_byte();
		pc++;
		uint8_t b2 = read_byte();
		pc++;
		uint16_t word = combine_bytes(b1, b2);
		if (word < 0x4269 || word > 0xFFFF)
		{
			printf("FATAL ERROR: Segmentation Fault\n");
			exit(139);
		}
		if ((memory[word] + registers[0]) > 0xFF)
		{
			registers[0] = 0xFF;
		}
		else
		{
			registers[0] += memory[word];
		}
	}
	printf("register after: %02hhX\n", registers[0]);
}
void OP3B_SUB()
{
	//sub from a register
	printf("register before: %02hhX | ", registers[0]);
	pc++;
	uint8_t ins = read_byte();
	if (ins == 0x00)
	{
		pc++;
		uint8_t b1 = read_byte();
		pc++;
		if ((registers[0] - b1) < 0x00)
		{
			registers[0] = 0x00;
		}
		else
		{
			registers[0] -= b1;
		}
	}
	else
	{
		uint8_t b1 = read_byte();
		pc++;
		uint8_t b2 = read_byte();
		pc++;
		uint16_t word = combine_bytes(b1, b2);
		if (word < 0x4269 || word > 0xFFFF)
		{
			printf("FATAL ERROR: Segmentation Fault\n");
			exit(139);
		}
		if ((registers[0] - memory[word]) < 0x00)
		{
			registers[0] = 0x00;
		}
		else
		{
			registers[0] -= memory[word];
		}
	}
	printf("register after: %02hhX\n", registers[0]);
}
//4- Instructions
void OP4A_TAX()
{
	//Transfer A register to X register
	registers[1] = registers[0];
	pc++;
	printf("X register: %02hhX\n", registers[1]);
}
void OP4B_TAY()
{
	//Transfer A register to Y register
	registers[2] = registers[0];
	pc++;
	printf("Y register: %02hhX\n", registers[2]);
}
void OP4C_TXA()
{
	//Transfer X register to A register
	registers[0] = registers[1];
	pc++;
	printf("A register: %02hhX\n", registers[0]);
}
void OP4D_TYA()
{
	//Transfer Y register to A register
	registers[0] = registers[2];
	pc++;
	printf("A register: %02hhX\n", registers[0]);
}
//5- Instructions
void OP5A_BNE()
{
	//BNE - BNE $JMP_ADDR, $ or #
	// "BNE $0x1B, #0x42"
	// So, This branches to address 0x1B if 0x42 != a register value
	pc++;
	uint8_t b1 = read_byte();
	pc++;
	uint8_t b2 = read_byte();
	pc++;
	uint16_t word = combine_bytes(b1, b2);
	
	uint8_t ins = read_byte();
	if (ins == 0x00)
	{
		pc++;
		uint8_t b1 = read_byte();
		pc++;
		if (b1 != registers[0])
		{
			pc = word;
		}
	}
	else
	{
		uint8_t bb1 = read_byte();
		pc++;
		uint8_t bb2 = read_byte();
		pc++;
		uint16_t wword = combine_bytes(bb1, bb2);
		if (word < 0x4269 || word > 0xFFFF)
		{
			printf("FATAL ERROR: Segmentation Fault\n");
			exit(139);
		}
		if (memory[wword] != registers[0])
		{
			pc = word;
		}
	}
}

void decode(uint8_t OP)
{
	switch(OP) 
	{
		case 0x0A:
			OP0A_NOP();
			break;
		case 0x0B:
			OP0B_JMP();
			break;
		case 0x1A:
			OP1A_RSA();
			break;
		case 0x1B:
			OP1B_LDA();
			break;
		case 0x1C:
			OP1C_STA();
			break;
		case 0x2A:
			OP2A_PHC();
			break;
		case 0x2B:
			OP2B_PPC();
			break;
		case 0x2C:
			OP2C_PHA();
			break;
		case 0x2D:
			OP2D_PPA();
			break;
		case 0x3A:
			OP3A_ADD();
			break;
		case 0x3B:
			OP3B_SUB();
			break;
		case 0x4A:
			OP4A_TAX();
			break;
		case 0x4B:
			OP4B_TAY();
			break;
		case 0x4C:
			OP4C_TXA();
			break;
		case 0x4D:
			OP4D_TYA();
			break;
		case 0x5A:
			OP5A_BNE();
			break;
		default:
			printf("FATAL ERROR: Unknown or illegal code.\n");
			exit(132);
	}
}
#endif