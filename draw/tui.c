#include "tui.h"

//prepare terminal ui
int tui_init( tui_t *t )
{
	int ret = -1;
	//should be empty pointer
	if (t != NULL)
		return -1;
	return ret;
}


//init waterfall
int tui_waterfall( tui_t *t, tui_waterfall_t *w )
{
	int ret=-1;
	//waterfall should be NULL
	if ( w != NULL )
		return -1;
	return ret;
}


//first draw, draw all buffer
int tui_waterfall_draw( tui_waterfall_t *w )
{
	int ret = -1;
	return ret;
}


//redraw only changed lines
int tui_waterfall_redraw( tui_waterfall_t *w )
{
	int ret = -1;
	return ret;
}


//update params of waterfall and then need to draw not redraw
int tui_waterfall_update( tui_waterfall_t *w )
{
	int ret = -1;
	return ret;
}


//push one line of data to buffer
int tui_waterfall_data( tui_waterfall_t *w, size_t *len, size_t *buf )
{
	int ret = -1;
	return ret;
}


//close terminal ui
int tui_close( tui_t *t )
{
	int ret = -1;
	//shouldnt be empty pointer
	if ( t == NULL )
		return -1;
	return ret;
}
