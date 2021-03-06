
#include "space_invaders.h"

#define SCREEN_WIDTH 224
#define SCREEN_HEIGHT 256
#define NUM_CYCLES 33333
/* Shift register for space invaders */
static uint16_t shift_reg = 0;
static uint8_t shift_offset = 0;

/* SDL variables */

static SDL_Surface * screen;
static SDL_Event ev;
static uint8_t * temp_screen;


union 
{
	uint8_t dip1;
	struct
	{
		uint8_t coin:1;
		uint8_t p2_start_button:1;
		uint8_t p1_start_button:1;
		uint8_t unused:1;
		uint8_t p1_shoot:1;
		uint8_t p1_joystick_left:1;
		uint8_t p1_joystick_right:1;
		uint8_t pad:1;
	};
}port1;

union
{
	uint8_t dip2;
	struct
	{
	 	uint8_t num_lives:2;
		uint8_t tilt_button:1;
		uint8_t bonus_life:1;
		uint8_t p2_shoot:1;
		uint8_t p2_joystick_left:1;
		uint8_t p2_joystick_right:1;
		uint8_t dipswitch_coin_info:1;
	};
}port2;

uint8_t space_invaders_port_in(uint8_t port, State8080* state)
{
	state->pc++;
	switch(port)
	{
		case 1:
			return port1.dip1;
			
		case 2:
			return port2.dip2;

		case 3:
			return (shift_reg <<  shift_offset) >> 8;	
		default:
			printf("Invalid port");
			exit(-1);
			return 0;
	}
}


void space_invaders_port_out(uint8_t port, State8080* state)
{
	state->pc++;
	switch(port)
	{
		case 2:
			shift_offset = (state->a & 0x7);
			break;

		case 4:
			shift_reg = (shift_reg >> 8) | (state->a << 8);
			break;

		/* Do nothing for port 3,  5, 6 (Not essential) */
		case 3:
		case 5:
		case 6:
			break;
		default:
			printf("Invalid port %d", port);
			exit(-1);

	}
}


/* Handle inputs */
void handle_input()
{
	while(SDL_PollEvent(&ev))
	{
		switch(ev.type)
		{
			case SDL_KEYDOWN:
				switch(ev.key.keysym.sym)
				{
					case SDLK_LEFT:
						port1.p1_joystick_left = 1;
						break;

					case SDLK_RIGHT:
						port1.p1_joystick_right = 1;
						break;

					case SDLK_RETURN:
						port1.p1_start_button = 1;
						break;

					case SDLK_SPACE:
						port1.p1_shoot = 1;
						break;

					case SDLK_c:
						port1.coin = 1;
						break;

					case SDLK_ESCAPE:
						exit(0);
						break;

					default:
						break;
				}	
				break;

			case SDL_KEYUP:
				switch(ev.key.keysym.sym)
				{
					case SDLK_LEFT:
						port1.p1_joystick_left = 0;
						break;

					case SDLK_RIGHT:
						port1.p1_joystick_right = 0;
						break;

					case SDLK_RETURN:
						port1.p1_start_button = 0;
						break;

					case SDLK_SPACE:
						port1.p1_shoot = 0;
						break;

					case SDLK_c:
						port1.coin= 0;
						break;

					default:
						break;
				}	
				break;
			case SDL_QUIT:
				exit(0);
				break;

			default:
				break;
		}
			
	}
}


void draw_screen(State8080 * state)
{
	int vmem_ptr;
	int i, j;

	SDL_LockSurface(screen);
	uint8_t *screen_ptr = screen->pixels;

	/* look at every bit in vmem (0x2400-0x4000) */
	for(vmem_ptr = 0x2400 ; vmem_ptr < 0x4000; vmem_ptr++)
	{
		for( i = 0; i < 8; i++)
		{
			temp_screen[(vmem_ptr - 0x2400)*8 + i] = ((state->memory[vmem_ptr] >> i) & 1) ? 0xFF: 0;
		}
	}


	/* Rotate the image */
	for(i = SCREEN_HEIGHT - 1; i >= 0; i--)
	{
		for ( j = 0; j < SCREEN_WIDTH; j++)
		{
			*screen_ptr = temp_screen[j* SCREEN_HEIGHT + i];
			screen_ptr++;
		}
	}
	SDL_UnlockSurface(screen);
	SDL_Flip(screen);
}

int main(int argc, char ** argv)
{

	State8080 * p_state = malloc(sizeof(State8080));	
	/* Clear the state */
	memset(p_state,0, sizeof(State8080));
	
	/* Allocate the memory, and begin reading from the binary */
	p_state->memory = malloc(0x5000);
	memset(p_state->memory, 0, 0x5000);
		
	char filename[] = "invaders";
	FILE * fptr = fopen(filename, "rb");
	if(fptr == NULL)
	{
		printf("Error: Could not open %s\n", filename);
		exit(1);
	}

	/* Find the lenght of the file and copy it into the buffer */
	fseek(fptr, 0, SEEK_END);
	int fsize = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	
	fread(p_state->memory, fsize, 1, fptr);
	fclose(fptr);

	p_state->pc = 0;

	/* Assign port IN and OUT */
	port8080_out = space_invaders_port_out;
	port8080_in = space_invaders_port_in;


	/* Initialize graphics */
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Could not initialize SDL");
		exit(-1);
	}
	atexit(SDL_Quit);


	temp_screen = malloc(SCREEN_WIDTH * SCREEN_HEIGHT);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,  8, SDL_DOUBLEBUF);

	/* Main loop */
	while(1)
	{
		emulate8080(p_state,(NUM_CYCLES * 6) / 7);
		generate_interrupt(p_state, 1);
		draw_screen(p_state);
		emulate8080(p_state, NUM_CYCLES / 7);
		generate_interrupt(p_state, 2);		
		handle_input();
		SDL_Delay(15);

	}
	return 0;

}

