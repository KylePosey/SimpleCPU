#include <windows.h>
#include "cpu.h"

/*const uint8_t basic_rom[0x3E80] = { 
0x0A, 0x1B, 0x00, 0x2C, 0x1C, 0x43, 0xFF, 0x1A, 
0x1B, 0x43, 0xFF, 0x3A, 0x00, 0x25, 0x3A, 0x43, 
0xFF, 0x3B, 0x00, 0x05, 0x3B, 0x43, 0xFF, 0x4A, 
0x4B, 0x2C, 0x2D };*/

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Must supply a file\n");
		return -1;
	}

	FILE *fp;
	fp = fopen(argv[1], "rb");
	if (!fp)
	{
		printf("Input file does not exist\n");
		return -1;
	}
	fclose(fp);
	init_cpu(fp);

	while(1)
	{
		if (pc > 0x3E80 || pc < 0x0000)
			exit(0);
		uint8_t ins = read_byte();
		if (ins == 0x00)
			exit(0);

		//printf("%02hhX\n", ins);
		//printf("PC: %02hhX\n", pc);
		
		decode(ins);

		Sleep(10);
	}

	return 0;
}