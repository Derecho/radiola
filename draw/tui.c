#include "tui.h"

#define T_ESC "\x1b"

//prepare terminal ui
int tui_init( tui_t **t )
{
	int ret = -1;
	tui_t *tui=NULL;

	//should be empty pointer
	if (*t != NULL)
		return -1;

	tui = malloc( sizeof(tui_t) );
	if ( tui == NULL )
		return -1;

	memset( tui, 0, sizeof( tui_t ) );

	tui->ifd = STDIN_FILENO;
	tui->ofd = STDOUT_FILENO;

	//if you whant raw mode then you should set it man
	if ( tcgetattr( tui->ifd, &tui->orig_i ) == -1 ) 
		goto exit_error;
	tui->raw_i = tui->orig_i;

	if ( tcgetattr( tui->ofd, &tui->orig_o ) == -1 ) 
		goto exit_error;
	tui->raw_o = tui->orig_o;

	//set not to echo output
	/* input modes: no break, no CR to NL, no parity check, no strip char,
	 * no start/stop output control. */
	tui->raw_i.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	/* output modes - disable post raw */
	tui->raw_i.c_oflag &= ~(OPOST);
	/* control modes - set 8 bit chars */
	tui->raw_i.c_cflag |= (CS8);
	/* local modes - choing off, canonical off, no extended functions,
	 * no signal chars (^Z,^C) */
	tui->raw_i.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	/* control chars - set return condition: min number of bytes and timer.
	 * We want read to return every single byte, without timeout. */

	/* put terminal in raw mode after flushing */
	if (tcsetattr( tui->ifd, TCSAFLUSH, &tui->raw_i) < 0)
	{
		//ERROR("Cannot set new terminal input attribures\n");
		goto exit_error;
	}

	*t = tui;
	ret = 0;

	return ret;

exit_error:
	free( tui );
	return -1;
}


//init waterfall
int tui_waterfall( tui_t **t, tui_waterfall_t **w )
{
	int ret=-1;
	tui_waterfall_t *wtf = NULL;

	//waterfall should be NULL
	if ( *w != NULL )
		return -1;


	wtf = malloc( sizeof(tui_waterfall_t) );
	if ( wtf == NULL )
	{
		return -1;
	}

	memset( wtf, 0, sizeof(tui_waterfall_t) );

	*w = wtf;
	(*t)->wf = wtf;
	ret = 0;

	return ret;

//exit_error:

//	return -1;
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
int tui_waterfall_update( tui_t *t )
{
	int ret = -1;
	int row=-1,col=-1;
	char buf[32];
	int i;

	//we trust that all params are ok

	/* go to right marging and get position */
	if ( write( t->ofd, "\x1b[999C", 6 ) != 6 )
		goto exit_error;

	if ( write( t->ofd, "\x1b[6n", 4 ) != 4 ) 
		goto exit_error;

	i = 0;
	//printf("here=%d\n", sizeof(buf));
	while (i < sizeof(buf)-1)
	{
		if ( read( t->ifd, buf+i,1 ) != 1 ) break;
		if (buf[i] == 'R') break;
		i++;
	}
	buf[i] = '\0';
	//printf("i=%d,buf=[%s]\n",i,buf);

	if ( buf[0] != '\x1b' || buf[1] != '[' ) 
	{
		goto exit_error;
	}

	//printf("i=%d,buf=[%s]\n",i,buf);

	if ( sscanf( buf+2, "%d;%d", &row, &col) != 2 ) 
		goto exit_error;

	//write( t->ofd, "\x1b[1C", 4 );
	write( t->ofd, T_ESC "[H" T_ESC "[2J", 7 );
	write( t->ofd, T_ESC "[0;0H", 6);

	t->wf->w = col;

	ret = 0;

	return ret;

exit_error:

	return -1;
}


//push one line of data to buffer
int tui_waterfall_data( tui_t *t, int len, uint8_t *buf )
{
	int ret = -1;
	int i;



	i = 0;
	while ( (i< t->wf->w) && ( i<len) )
	{
		//printf("-%d", buf[i]);
		uint8_t c = tui_waterfall_color( buf[i] );
		char buf[32];
		snprintf( buf, 32, T_ESC "[48;5;%dm " T_ESC "[0m",c);
		write( t->ofd, buf, strlen(buf) );
		i++;
	}

	return ret;
}

uint8_t tui_waterfall_color( uint8_t d )
{
	
	uint8_t color=15;
	
	/*
	if ( d < 50 )
	{
		color = 17;
	} else if ( d < 100 )
	{
		color = 18;
	} else if ( d < 150 )
	{
		color = 19;
	} else if ( d < 200 )
	{
		color = 20;
	} else
	{
		color = 21;
	}
	*/
	
	
	if ( d == 0 )
	{
		color = 17;
	} else if ( d == 1 )
	{
		color = 18;
	} else if ( d == 2 )
	{
		color = 19;
	} else if ( d == 3 )
	{
		color = 20;
	} else if ( d == 4 )
	{
		color = 21;
	} else if ( d == 5 )
	{
		color = 26;
	} else if ( d == 6 )
	{
		color = 27;
	} else if ( d == 7 )
	{
		color = 44;
	} else
	{
		color = 230;
	}
	
	/*
	uint8_t col[] = { 16,17,18,19,20,21,26,27,44,45,86,87,230,229,228,227,226,214,202,196,160,124,88,52 };
	int len = 24;
	int step = 256/len;

	color = col[d/step];
	*/

	return color;
}


//close terminal ui
int tui_close( tui_t *t )
{
	int ret = -1;
	
	//shouldnt be empty pointer
	if ( t == NULL )
		return -1;
	
	//restore terminal mode after closing 
	tcsetattr( t->ifd, TCSAFLUSH, &t->orig_i );

	return ret;
}
