#include "emulator.h"
#include <stdio.h>

int main(int argc, char ** argv)
{
	State8080 * p_state = malloc(sizeof(State8080));	
	/* Clear the state */
	memset(p_state,0, sizeof(State8080));
	
	/* Allocate the memory, and begin reading from the binary */
	p_state->memory = malloc(0x4000);
		
	FILE * fptr = fopen(argv[1], "rb");
	if(fptr == NULL)
	{
		printf("Error: Could not open %s\n", argv[1]);
		exit(1);
	}

	/* Find the lenght of the file and copy it into the buffer */
	fseek(fptr, 0, SEEK_END);
	int fsize = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	
	fread(p_state->memory+ 0x100, fsize, 1, fptr);
	fclose(fptr);

	p_state->pc = 0x100;
			
	p_state->memory[0x59c] = 0xc3;
	p_state->memory[0x59d] = 0xc2;
	p_state->memory[0x59e] = 0x05;
	while(1)
	{
		emulate8080(p_state, 1);
	}

}
