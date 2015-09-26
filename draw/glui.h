#ifndef __RADIOLA_GLUI_H
#define __RADIOLA_GLUI_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

//to draw waterfall
typedef struct glui_waterfall_t
{
	int type;
	int h,w;
	uint8_t *buf;
	size_t buf_len;
} glui_waterfall_t;

typedef struct glui_t
{
	glui_waterfall_t *wf;
} glui_t;

//prepare terminal ui
int glui_init( glui_t **t );
//init waterfall
int glui_waterfall( glui_t **t, glui_waterfall_t **w );
//first draw, draw all buffer
int glui_waterfall_draw( glui_waterfall_t *w );
//redraw only changed lines
int glui_waterfall_redraw( glui_waterfall_t *w );
//update params of waterfall and then need to draw not redraw
int glui_waterfall_update( glui_t *w );
//push one line of data to buffer
int glui_waterfall_data( glui_t *w, int len, uint8_t *buf );
//return color
uint8_t glui_waterfall_color( uint8_t d );
//close terminal ui
int glui_close( glui_t *t );