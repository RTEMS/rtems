/*
 *  $Id$
 */

#include <go32.h>
#include <bsp.h>

#include <string.h>

#define	MAX_COL	80
#define MAX_ROW	50

static unsigned		nrow		= 25;
static unsigned		ncol		= 80;
static unsigned short *	tvram		= TVRAM;
static unsigned char	current_col	= 0;
static unsigned char	current_row	= 0;
static unsigned short	screen_copy[ MAX_ROW*MAX_COL ];

static void init_cons( void );

/*
 * set_cursor_pos()
 *	Set cursor position based on current absolute screen offset
 */
static void
set_cursor_pos(void)
{
    register unsigned short gdc_pos = current_row * ncol + current_col;
    outport_byte( GDC_REG_PORT, 0xe );
    outport_byte( GDC_VAL_PORT, (gdc_pos >> 8) & 0xff );
    outport_byte( GDC_REG_PORT, 0xf );
    outport_byte( GDC_VAL_PORT, gdc_pos & 0xff );
}

/*
 * scroll_up()
 *	Scroll screen up one line
 */
static void
scroll_up( unsigned short * tv, unsigned short * copy, unsigned int lines )
{
    if ( lines > nrow )
	lines = nrow;

    /* move everything up */
    memmove( copy, copy+ncol*lines, (nrow-lines)*ncol*sizeof copy[0] );

    /* fill bottom with blanks */
    {
	int loop = ncol*lines;
	unsigned short * ptr = copy + ncol*(nrow-lines);
	while ( --loop >= 0 )
	    *ptr++ = (WHITE<<8) | ' ';
    }

    /* copy new screen to video buffer */
    dosmemput( copy, nrow*ncol*sizeof copy[0], (int)tv );
}


/*
 * PUT()
 *	Write character at current screen location
 */
inline static void PUT( char c )
{
    unsigned short loc = current_row*ncol+current_col;
    unsigned short val = (WHITE<<8) | c;
    screen_copy[loc] = val;
    dosmemput( &screen_copy[loc], sizeof screen_copy[0], (int)(tvram+loc) );
}

/*
 * cons_putc()
 *	Place a character on next screen position
 */
static void
cons_putc( unsigned char c )
{
    static int first = 1;
    if ( first )  {
	init_cons();
	first = 0;
    }

	switch (c) {
	case '\t':
		while ( current_row % 8 )
			cons_putc(' ');
		break;
	case '\r':
		current_col = 0;
		break;
	case '\n':
		if ( ++current_row >= nrow ) {
		        scroll_up( tvram, screen_copy, 1 );
			current_row -= 1;
		}
		break;
	case '\b':
		if ( current_col > 0 ) {
			--current_col;
			PUT(' ');
		}
		break;
	default:
		PUT(c);
		current_col += 1;
		if ( current_col >= ncol ) {
			current_col = 0;
			current_row += 1;
			if ( current_row >= nrow ) {
			        scroll_up( tvram, screen_copy, 1 );
				current_row -= 1;
			}
		}
	};
	set_cursor_pos();
}


/*
 * init_cons()
 *	Hook for any early setup
 */
static void
init_cons( void )
{
#if 0
    /* Get a copy of original screen */
    dosmemget( (int)tvram, nrow*ncol*sizeof *tvram, screen_copy );
#else
    /* Clear entire screen */
    scroll_up( tvram, screen_copy, nrow );
#endif
}



void _IBMPC_outch( unsigned char ch )
{
    extern int _IBMPC_Use_Go32_IO;

    if ( _IBMPC_Use_Go32_IO )  {
	write( 1, &ch, 1 );
    } else {
	cons_putc( ch );
    }
}
