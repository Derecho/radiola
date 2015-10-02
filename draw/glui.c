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

	tui->h = SCREEN_Y;
	tui->w = SCREEN_X;
	tui->win = SDL_CreateWindow("Hello World!", tui->w, tui->h, SCREEN_X, SCREEN_Y, SDL_WINDOW_SHOWN);
	if (tui->win == NULL)
	{
		printf("Couldnt create SDL window\n");
		return -1;
	}

	*t = tui;
	ret = 0;

	return ret;
}


//init waterfall
int glui_waterfall( glui_t **t, glui_waterfall_t **w )
{
	int ret=-1;
	
	glui_waterfall_t *wtf = NULL;

	wtf = malloc( sizeof(glui_waterfall_t) );
	if ( wtf == NULL )
	{
		printf("Cannot alloc waterfall\n");
		return -1;
	}

	memset( wtf, 0, sizeof(glui_waterfall_t) );

	wtf->h = (*t)->h;
	wtf->w = (*t)->w;
	wtf->cur_h = 5;


	wtf->rend = SDL_CreateRenderer( (*t)->win, -1, SDL_RENDERER_ACCELERATED);
	if ( wtf->rend == NULL )
	{
		printf("Canno create SDL Rendered\n");
		return -1;
	}

	(*w) = wtf;
	(*t)->wf = wtf;

	ret = 0;

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
int glui_waterfall_data( glui_t *t, int len, uint8_t *buf )
{
	int ret=-1;
	int i;
	int y;
	glui_color_t c = glui_waterfall_color(0);
	SDL_Point *pt = NULL;

	SDL_SetRenderDrawColor( t->wf->rend, c.r, c.g, c.b, c.a );
	
	y = t->wf->cur_h;
	t->wf->cur_h += 1;
	pt = malloc( sizeof(SDL_Point) );
	for ( i=0; i<len; i++ )
	{
		c = glui_waterfall_color(buf[i]);
		SDL_SetRenderDrawColor( t->wf->rend, c.r, c.g, c.b, c.a );
		pt[0].x = i;
		pt[0].y = y;
		SDL_RenderDrawPoints( t->wf->rend, pt, 1 );	
	}

	
	SDL_RenderPresent( t->wf->rend );
	ret = 0;

	return ret;
}


//return color
glui_color_t glui_waterfall_color( uint8_t d )
{
	glui_color_t c;
	
	c.r = d;
	c.g = d;
	c.b = d;

	return c;
}


//close terminal ui
int glui_close( glui_t *t )
{
	int ret=0;
	

	if ( t->win != NULL )
	{
		SDL_DestroyWindow( t->win );
	}

	SDL_Quit();


	return ret;
}