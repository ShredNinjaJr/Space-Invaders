#include <SDL.h>
#include <stdio.h>


/* Main */
int main(int argc, char ** argv)
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
		fprintf(stderr,"Could not initialize SDL: %s\n", SDL_GetError());
	printf("SDL Initialized\n");

	SDL_Quit();

	printf("SDL Shutdown\n");

	return 0;
}
