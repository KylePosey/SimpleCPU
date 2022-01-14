#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

int op_decode(char* ins, char* r_ins);
uint8_t op_lookup(char* ins);

int main(int argc, char **argv)
{
    uint8_t bytes[0x3E80] = {};
    int byte_index = 0;

    FILE *fp_read;
	fp_read = fopen(argv[1], "rb");
	if (!fp_read)
	{
		printf("Input file does not exist\n");
		return -1;
	}

	fseek(fp_read, 0, SEEK_END);
	long fsize = ftell(fp_read);
	fseek(fp_read, 0, SEEK_SET); 

	int line_index = 1;

	char *file_buffer = malloc(fsize + 1);
	for (int i = 0; i < fsize; i++)
	{
		fread(file_buffer + i, fsize, 1, fp_read);
		if (file_buffer[i] == '\n')
		{
			line_index++;
		}
	}
	file_buffer[fsize] = 0;
	fclose(fp_read);

	//TODO Find what maximum character size is and use that instead for effiency
	char buffer[line_index][50];
	memset(buffer, 0, line_index*50*sizeof(char));
	int last_index = 0;
	for (int j = 0; j < line_index; j++)
	{
		for (int x = last_index; x < fsize; ++x)
		{
			if (file_buffer[x] != '\n')
			{
				buffer[j][x - last_index] = toupper(file_buffer[x]);
			}
			else
			{
				last_index = x + 1;
				x = fsize + 1;
			}
		}
		buffer[j][strcspn(buffer[j], "\n")] = '\0';
	}

	char delim[] = " ";
	for (int y = 0; y < line_index; y++)
	{
		char *ptr = strtok(buffer[y], delim);
		while (ptr != NULL)
		{
			uint8_t b = op_lookup(ptr);
			if (b != 0x00)
			{
				bytes[byte_index] = b;
				printf("%02hhX\n", b);
			}
			else
			{
				size_t le = strlen(ptr);
				for (int i = 0; i < le; i++)
				{
					if (ptr[i] == '\0')
					{
						goto b_term;
					}
					if (ptr[i] == '#')
					{
						bytes[byte_index] = 0x00;
						byte_index++;
					}
					//else if ()		
				}

				b_term:
			}
			ptr = strtok(NULL, delim);
		}
	}

	printf("\n\ncompleted op\n");

	return 0;

	//TODO CHECK IF TOTAL BYTES EXCEEDS 16K

	FILE *fp_write;

	if (argc >= 3)
	{
		fp_write = fopen(argv[2], "w");
	}
	else
	{
		printf("Must supply a filename to create and write to\n");
		return -1;
	}

	for (int i = 0; i < 0x3E80; i++)
	{
		putc(bytes[i], fp_write);
	}

	fclose(fp_write);
	return 0;
}

int op_decode(char* ins, char* r_ins)
{
	//return 0 if true, 1 if false
	size_t length = strlen(ins);
	size_t max_ins = strlen(r_ins);
	if (length >= max_ins)
	{
		int i = 0;
		for (i = 0; i < max_ins; i++)
		{
			if (ins[i] != r_ins[i])
			{
				return 1;
			}
		}
		if (length > max_ins)
		{
			if (ins[i + 1] == '\0' || ins[i + 1] == '\n')// || ins[i + 1] == '')
			{
				return 0;
			}
			return 1;
		}
		return 0;
	}
	else
		return 1;
}

uint8_t op_lookup(char* ins)
{

	//printf("%s\n", ins);
	if (op_decode(ins, "NOP") == 0)
	{
		return 0x0A;
	}
	else if (op_decode(ins, "ADD") == 0)
	{
		return 0x3A;
	}
	else if (op_decode(ins, "LDA") == 0))
	{
		return 0x1B;
	}
	else
		return 0x00;
}