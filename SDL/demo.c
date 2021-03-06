/* Includes */
#include <SDL.h>
#include <time.h>
#include <stdlib.h>

#define NUM_DOTS 1024* 8
#define WINDOW_HEIGHT 1024
#define WINDOW_WIDTH 1280

typedef struct
{
	int red, green;
	float vx, vy;
	float x, y;
}dot;

/* Globals */
SDL_Surface *demo_screen;
float demo_time_measure = 0, demo_time_step = 0;
dot demo_dots[NUM_DOTS];


/* Returns a random float between 0 and 1 */
float demo_roll()
{
	float r;
	r = (float) (rand() % RAND_MAX);
	r /= (float)(RAND_MAX - 1);
	return r;
}

/*initialize dots */
void demo_init()
{
	int i;
	for(i = 0; i < NUM_DOTS; i++)
	{
		demo_dots[i].red = rand()%255;
		demo_dots[i].green = rand()%255;
      		demo_dots[i].vx = demo_roll()*16.0f-8.0f;
		demo_dots[i].vy = demo_roll()*16.0f-8.0f;
		demo_dots[i].x = demo_roll()*WINDOW_WIDTH;
		demo_dots[i].y = demo_roll()* WINDOW_HEIGHT;
	}
}


void demo_handle()
{
	int i;
	for(i = 0; i < NUM_DOTS; i++)
	{
		/* Move */
		demo_dots[i].x += demo_dots[i].vx * demo_time_step;
		demo_dots[i].y += demo_dots[i].vy * demo_time_step;
		/* Hit walls? */
		if(demo_dots[i].x < 0.0f || demo_dots[i].x >= WINDOW_WIDTH)
		{
			/* Undo move (demo_time_step is still the same value it was before and is valid for the current frame) */
			demo_dots[i].x -= demo_dots[i].vx*demo_time_step;
			/* Reverse */
			demo_dots[i].vx = -demo_dots[i].vx;
		}
		if(demo_dots[i].y < 0.0f || demo_dots[i].y >= WINDOW_HEIGHT)
		{
			/* Undo move (demo_time_step is still the same value it was before and is valid for the current frame) */
			demo_dots[i].y -= demo_dots[i].vy*demo_time_step;
			/* Reverse */
			demo_dots[i].vy = -demo_dots[i].vy;
		}
	}
}

/* Draw dots */
void demo_draw()
{
	int i,bpp,rank,x,y;
	Uint32 *pixel;
	/* Lock surface */
	SDL_LockSurface(demo_screen);
	rank = demo_screen->pitch/sizeof(Uint32);
	pixel = (Uint32*)demo_screen->pixels;
	/* Draw all dots */
	for(i = 0;i < NUM_DOTS;i++)
	{
		/* Rasterize position as integer */
		x = (int)demo_dots[i].x;
		y = (int)demo_dots[i].y;
		/* Set pixel */
		pixel[x+y*rank] = SDL_MapRGBA(demo_screen->format,demo_dots[i].red,demo_dots[i].green,0,255);
	}
	/* Unlock surface */
	SDL_UnlockSurface(demo_screen);
}

/* Convert from timespec to float */
float demo_convert_time(struct timespec *ts)
{
	float accu;
	/* Combine the value into floating number */
	accu = (float) ts->tv_sec; /* seconds that have gone by */
	accu *= 1000000000.0f; /* One second is 1 billion nano seconds */
	accu += (float) ts->tv_nsec;	/* nano seconds that have gone by */
	/* Bring it back to millisecond range but keeping the nano second resolution */
	accu /= 1000000.0f;

	return accu;
}

/* Start time */
void demo_start_time()
{
	struct timespec ts;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
	demo_time_measure = demo_convert_time(&ts);
}

/* End time */
void demo_end_time()
{
	struct timespec ts;
	float delta;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
	delta = demo_convert_time(&ts) - demo_time_measure; /* Find the distance in time */
	demo_time_step = delta/ (1000.0f/ 16.0f); /* Weird formula equals 1.0f at 16 frames a second */

}

/* Main */
int main(int argc,char **argv)
{
    SDL_Event ev;
    int active;
    /* Initialize SDL */
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
        fprintf(stderr,"Could not initialize SDL: %s\n",SDL_GetError());
    /* Open main window */
    demo_screen = SDL_SetVideoMode(WINDOW_WIDTH,WINDOW_HEIGHT,0,SDL_HWSURFACE|SDL_DOUBLEBUF);
    if(!demo_screen)
        fprintf(stderr,"Could not set video mode: %s\n",SDL_GetError());

    /* initialize game */
    demo_init();

    /* Main loop */
    active = 1;
    while(active)
    {
        /* Handle events */
        while(SDL_PollEvent(&ev))
        {
            if(ev.type == SDL_QUIT)
                active = 0; /* End */
        }
	demo_start_time();
	demo_handle();
	SDL_FillRect(demo_screen, NULL, SDL_MapRGBA(demo_screen->format,0,0,255,255));
	demo_draw();
	SDL_Flip(demo_screen);
	demo_end_time();
    }
    /* Exit */
    SDL_Quit();
    return 0;
}


