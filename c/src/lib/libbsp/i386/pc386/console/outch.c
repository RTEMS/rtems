/*
 * outch.c  - This file contains code for displaying characters
 *	      on the console uisng information that should be
 *	      maintained by the BIOS in its data Area.
 *
 * Copyright (C) 1998  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * $Id$
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
endColumn(void)
{
    if (++row == maxRow) { 
	scroll(); 	/* Scroll the screen now */
	row = maxRow - 1; 
    }
    column = 0;
    nLines++;
    /* Move cursor on the next location  */
    wr_cursor(row * maxCol + column, ioCrtBaseAddr);
}



static void 
videoPutChar(char car)
{
    unsigned short *pt_bitmap = bitMapBaseAddr + row * maxCol;
  
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
	    pt_bitmap += column;
	    column += i;
	    if (column >= maxCol) {
		endColumn();
		return;
	    }
	    while (i--)	*pt_bitmap++ = ' ' | attribute;		
	    wr_cursor(row * maxCol + column, ioCrtBaseAddr);
	    return;
	}
	case '\n': {
	    endColumn();
	    return;
	}
        case 7: {	/* Bell code must be inserted here */
	    return;
	}
	case '\r' : {   /* Already handled via \n */
	    return;
	}
    	default: {
	    pt_bitmap += column;
	    *pt_bitmap = car | attribute;
	    if (++column == maxCol) endColumn();
	    else wr_cursor(row * maxCol + column, 
			  ioCrtBaseAddr);
	    return;
	}
    }
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
  videoPutChar(c);
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
  row = x;
  column = y;
  wr_cursor(row * maxCol + column, ioCrtBaseAddr);
}


int whereX( void )
{
  return row;
}

int whereY( void )
{
  return column;
}

