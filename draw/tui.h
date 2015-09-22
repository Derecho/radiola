#ifndef __RADIOLA_TUI_H
#define __RADIOLA_TUI_H

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

//to draw waterfall
typedef struct tui_waterfall_t
{
	int type;
	int h,w;

} tui_waterfall_t;

typedef struct tui_t
{
	int ifd, ofd;
	struct termios orig_i, orig_o;
	struct termios raw_i, raw_o;
	tui_waterfall_t *wf;
} tui_t;

//prepare terminal ui
int tui_init( tui_t *t );
//init waterfall
int tui_waterfall( tui_t *t, tui_waterfall_t *w );
//first draw, draw all buffer
int tui_waterfall_draw( tui_waterfall_t *w );
//redraw only changed lines
int tui_waterfall_redraw( tui_waterfall_t *w );
//update params of waterfall and then need to draw not redraw
int tui_waterfall_update( tui_waterfall_t *w );
//push one line of data to buffer
int tui_waterfall_data( tui_waterfall_t *w, size_t *len, size_t *buf );
//close terminal ui
int tui_close( tui_t *t );

#endif