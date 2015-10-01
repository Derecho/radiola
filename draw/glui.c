#include "glui.h"

#define DEFAULT_TITLE "RADIOLA"
#define SCREEN_X 640
#define SCREEN_Y 480

int glui_init( glui_t **t )
{
	int ret=-1;

	glui_t *tui = NULL;

	tui = malloc( sizeof(glui_t) );
	if (tui == NULL)
	{
		return -1;
	}

	memset(tui, 0, sizeof(glui_t));

	if ( SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Cannot init sdl\n");
		return -1;
	}

	tui->win = SDL_CreateWindow("Hello World!", 100, 100, SCREEN_X, SCREEN_Y, SDL_WINDOW_SHOWN);
	if (tui->win == NULL)
	{
		printf("Couldnt create SDL window\n");
		return -1;
	}

	tui->rend = SDL_CreateRenderer( tui->win, -1, SDL_RENDERER_ACCELERATED);



	return ret;
}


//init waterfall
int glui_waterfall( glui_t **t, glui_waterfall_t **w )
{
	int ret=-1;
	

	return ret;
}


//first draw, draw all buffer
int glui_waterfall_draw( glui_waterfall_t *w )
{
	int ret=-1;
	

	return ret;
}


//redraw only changed lines
int glui_waterfall_redraw( glui_waterfall_t *w )
{
	int ret=-1;
	

	return ret;
}


//update params of waterfall and then need to draw not redraw
int glui_waterfall_update( glui_t *w )
{
	int ret=-1;
	

	return ret;
}


//push one line of data to buffer
int glui_waterfall_data( glui_t *w, int len, uint8_t *buf )
{
	int ret=-1;
	

	return ret;
}


//return color
uint8_t glui_waterfall_color( uint8_t d )
{
	uint8_t c;
	

	return c;
}


//close terminal ui
int glui_close( glui_t *t )
{
	int ret=0;
	
	if ( t->rend )
	{
		SDL_DestroyRenderer( t->rend );
	}

	if ( t->win != NULL )
	{
		SDL_DestroyWindow( t->win );
	}

	SDL_Quit();


	return ret;
}