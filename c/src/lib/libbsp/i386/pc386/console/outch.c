/*
 * outch.c  - This file contains code for displaying characters
 *	      on the console uisng information that should be
 *	      maintained by the BIOS in its data Area.
 *
 * Copyright (C) 1998  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * Till Straumann <strauman@slac.stanford.edu>, 2003/9:
 *  - added handling of basic escape sequences (cursor movement
 *    and erasing; just enough for the line editor 'libtecla' to
 *    work...)
 */

#include <bsp.h>

#include <stdlib.h>
#include <string.h>

#include <crt.h>

extern void wr_cursor(int, unsigned short);

#define TAB_SPACE 4
static unsigned short *bitMapBaseAddr;
static unsigned short ioCrtBaseAddr;
static unsigned short maxCol;
static unsigned short maxRow;
static unsigned char  row;
static unsigned char  column;
static unsigned short attribute;
static unsigned int   nLines;

static void
scroll(void)
{
    int i, j;				    /* Counters	*/
    unsigned short *pt_scroll, *pt_bitmap;  /* Pointers on the bit-map	*/

    pt_bitmap = bitMapBaseAddr;
    j = 0;
    pt_bitmap = pt_bitmap + j;
    pt_scroll = pt_bitmap + maxCol;
    for (i = j; i < (maxRow - 1) * maxCol; i++) {
	*pt_bitmap++ = *pt_scroll++;
    }

    /*
     * Blank characters are displayed on the last line.
     */
    for (i = 0; i < maxCol; i++) {
	*pt_bitmap++ = (short) (' ' | attribute);
    }
}

static void
doCRNL(int cr, int nl)
{
	if (nl) {
    if (++row == maxRow) {
	scroll(); 	/* Scroll the screen now */
	row = maxRow - 1;
    }
    nLines++;
	}
	if (cr)
    	column = 0;
    /* Move cursor on the next location  */
	if (cr || nl)
    	wr_cursor(row * maxCol + column, ioCrtBaseAddr);
}

int (*videoHook)(char, int *)=0;

static void
advanceCursor(void)
{
  if (++column == maxCol)
	doCRNL(1,1);
  else
	wr_cursor(row * maxCol + column, ioCrtBaseAddr);
}

static void
gotorc(int r, int c)
{
	column = c;
	row    = r;

  	wr_cursor(row * maxCol + column, ioCrtBaseAddr);
}

#define ESC		((char)27)
/* erase current location without moving the cursor */
#define BLANK	((char)0x7f)

static void
videoPutChar(char car)
{
    unsigned short *pt_bitmap = bitMapBaseAddr + row * maxCol + column;

    switch (car) {
    case '\b': {
	    if (column) column--;
	    /* Move cursor on the previous location  */
	    wr_cursor(row * maxCol + column, ioCrtBaseAddr);
	    return;
	}
	case '\t': {
	    int i;

	    i = TAB_SPACE - (column & (TAB_SPACE - 1));
	    column += i;
	    if (column >= maxCol) {
		doCRNL(1,1);
		return;
	    }
	    while (i--)	*pt_bitmap++ = ' ' | attribute;
	    wr_cursor(row * maxCol + column, ioCrtBaseAddr);
	    return;
	}
	case '\n': {
	    doCRNL(0,1);
	    return;
	}
    case 7:		{	/* Bell code must be inserted here */
	    return;
	}
	case '\r' : {
		doCRNL(1,0);
	    return;
	}
	case BLANK: {
	    *pt_bitmap = ' ' | attribute;
		/* DONT move the cursor... */
		return;
	}
   	default: {
	    *pt_bitmap = (unsigned char)car | attribute;
		advanceCursor();
	    return;
	}
    }
}

/* trivial state machine to handle escape sequences:
 *
 *                    ---------------------------------
 *                   |                                 |
 *                   |                                 |
 * KEY:        esc   V    [          DCABHKJ       esc |
 * STATE:   0 -----> 27 -----> '[' ----------> -1 -----
 *          ^\        \          \               \
 * KEY:     | \other   \ other    \ other         \ other
 *           <-------------------------------------
 *
 * in state '-1', the DCABHKJ cases are handled
 *
 * (cursor motion and screen clearing)
 */

#define DONE  (-1)

static int
handleEscape(int oldState, char car)
{
int rval = 0;
int ro,co;

	switch ( oldState ) {
		case DONE:	/*  means the previous char terminated an ESC sequence... */
		case 0:
			if ( 27 == car ) {
				rval = 27;	 /* START of an ESC sequence */
			}
		break;

		case 27:
			if ( '[' == car ) {
				rval = car;  /* received ESC '[', so far */
			} else {
				/* dump suppressed 'ESC'; outch will append the char */
				videoPutChar(ESC);
			}
		break;

		case '[':
			/* handle 'ESC' '[' sequences here */
			ro = row; co = column;
			rval = DONE; /* done */

			switch (car) {
				case 'D': /* left */
					if ( co > 0 )      co--;
				break;
				case 'C': /* right */
					if ( co < maxCol ) co++;
				break;
				case 'A': /* up    */
					if ( ro > 0 )      ro--;
				break;
				case 'B': /* down */
					if ( ro < maxRow ) ro++;
				break;
				case 'H': /* home */
					ro = co = 0;
				break;
				case 'K': /* clear to end of line */
					while ( column < maxCol - 1 )
                		videoPutChar(' ');
            		videoPutChar(BLANK);
        		break;
        		case 'J': /* clear to end of screen */
					while (  ((row < maxRow-1) || (column < maxCol-1)) )
						videoPutChar(' ');
					videoPutChar(BLANK);
        		break;
        		default:
            		videoPutChar(ESC);
            		videoPutChar('[');
					/* DONT move the cursor */
					ro   = -1;
					rval = 0;
        		break;
			}
			/* reset cursor */
			if ( ro >= 0)
				gotorc(ro,co);

		default:
		break;

	}

	return rval;
}

void
clear_screen(void)
{
    int i,j;

    for (j = 0; j <= maxRow; j++) {
      for (i = 0; i <= maxCol; i++) {
	videoPutChar(' ');
      }
    }
    column  = 0;
    row     = 0;
}

/*-------------------------------------------------------------------------+
|         Function: _IBMPC_outch
|      Description: Higher level (console) interface to consPutc.
| Global Variables: None.
|        Arguments: c - character to write to console.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
void
_IBMPC_outch(char c)
{
static int escaped = 0;

  if ( ! (escaped = handleEscape(escaped, c)) ) {
    if ( '\n' == c )
      videoPutChar('\r');
  	videoPutChar(c);
  }
} /* _IBMPC_outch */

/*-------------------------------------------------------------------------+
|         Function: _IBMPC_initVideo
|      Description: Video system initialization. Hook for any early setup.
| Global Variables: bitMapBaseAddr, ioCrtBaseAddr, maxCol, maxRow, row
|		    column, attribute, nLines;
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
void
_IBMPC_initVideo(void)
{
    unsigned char* pt = (unsigned char*) (VIDEO_MODE_ADDR);

    if (*pt == VGAMODE7) {
      bitMapBaseAddr = (unsigned short*) V_MONO;
    }
    else {
      bitMapBaseAddr = (unsigned short*) V_COLOR;
    }
    ioCrtBaseAddr = *(unsigned short*) DISPLAY_CRT_BASE_IO_ADDR;
    maxCol  = * (unsigned short*) NB_MAX_COL_ADDR;
    maxRow  = * (unsigned char*)  NB_MAX_ROW_ADDR;
    column  = 0;
    row     = 0;
    attribute = ((BLACK << 4) | WHITE)<<8;
    nLines = 0;
    clear_screen();
#ifdef DEBUG_EARLY_STAGE
    printk("bitMapBaseAddr = %X, display controller base IO = %X\n",
	   (unsigned) bitMapBaseAddr,
	   (unsigned) ioCrtBaseAddr);
    videoPrintf("maxCol = %d, maxRow = %d\n", (unsigned) maxCol, (unsigned) maxRow);
#endif
} /* _IBMPC_initVideo */

/* for old DOS compatibility n-curses type of applications */
void gotoxy( int x, int y )
{
  gotorc(y,x);
}

int whereX( void )
{
  return row;
}

int whereY( void )
{
  return column;
}
