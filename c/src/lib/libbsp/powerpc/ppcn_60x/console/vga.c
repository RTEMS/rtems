/*
 *  This file contains the TTY driver for VGA
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 *  This driver uses the termios pseudo driver.
 */
/*-------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   outch.c,v 1.4 1995/12/19 20:07:27 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| * COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.                      *
| * On-Line Applications Research Corporation (OAR).                       *
| **************************************************************************
+--------------------------------------------------------------------------*/


#include <bsp.h>

#include <stdlib.h>
#include <string.h>

#include "vga_p.h"

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define DISPLAY_CELL_COUNT (VGA_NUM_ROWS * VGA_NUM_COLS)
                                       /* Number of display cells.            */
#define TABSIZE 4                      /* Number of spaces for TAB (\t) char. */
#define	WHITE	0x0007                 /* White on Black background colour.   */
#define BLANK   (WHITE | (' '<<8))     /* Blank character.                    */

/*
 * This is imported from i8042.c to provide flow control
 */
extern volatile boolean bScrollLock;

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
/* Physical address of start of video text memory. */
static unsigned16 *videoRam    = (unsigned16 *)VGA_FB;
/* Pointer for current output position in display. */
static unsigned16 *videoRamPtr = (unsigned16 *)VGA_FB;
static unsigned8  videoRows = VGA_NUM_ROWS; /* Number of rows in display.    */
static unsigned8  videoCols = VGA_NUM_COLS; /* Number of columns in display. */
static unsigned8  cursRow   = 0;       /* Current cursor row.           */
static unsigned8  cursCol   = 0;       /* Current cursor column.        */


/*-------------------------------------------------------------------------+
|         Function: setHardwareCursorPos
|      Description: Set hardware video cursor at given offset into video RAM. 
| Global Variables: None.
|        Arguments: videoCursor - Offset into video memory.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static inline void
setHardwareCursorPos(unsigned16 videoCursor)
{
	VGA_WRITE_CRTC(0x0e, (videoCursor >> 8) & 0xff);
	VGA_WRITE_CRTC(0x0f, videoCursor & 0xff);
} /* setHardwareCursorPos */


/*-------------------------------------------------------------------------+
|         Function: updateVideoRamPtr
|      Description: Updates value of global variable "videoRamPtr" based on
|                   current window's cursor position. 
| Global Variables: videoRamPtr, cursRow, cursCol.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static inline void
updateVideoRamPtr(void)
{
	videoRamPtr = videoRam + cursRow * videoCols + cursCol;
} /* updateVideoRamPtr */


/*-------------------------------------------------------------------------+
|         Function: scrollUp
|      Description: Scrolls display up n lines.
| Global Variables: None.
|        Arguments: lines - number of lines to scroll.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static void
scrollUp(unsigned8 lines)
{
	/* Number of blank display cells on bottom of window. */
	unsigned16 blankCount;

       /* Source and destination pointers for memory copy operations. */
	unsigned16 *ptrDst, *ptrSrc;

	if(lines<videoRows)  /* Move window's contents up. */
	{
		/*
		 * Number of non-blank cells on upper part
		 * of display (total - blank).
		 */
		unsigned16 nonBlankCount;

		blankCount = lines * videoCols;
		nonBlankCount = DISPLAY_CELL_COUNT - blankCount;
		ptrSrc = videoRam + blankCount;
		ptrDst = videoRam; 

		while(nonBlankCount--)
		{
			*ptrDst++ = *ptrSrc++;
		}
	}
	else
	{
		/* 
		 * Clear the whole display.
		 */
		blankCount = DISPLAY_CELL_COUNT;
		ptrDst = videoRam;
	}

	/* Fill bottom with blanks. */
	while (blankCount-->0)
	{
		*ptrDst++ = BLANK;
	}
} /* scrollUp */


/*-------------------------------------------------------------------------+
|         Function: printCHAR
|      Description: Print printable character to display.
| Global Variables: videoRamPtr, cursRow, cursCol.
|        Arguments: c - character to write to display.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static void
printCHAR(char c)
{
	*videoRamPtr++ = (c<<8) | WHITE;
	cursCol++;
	if(cursCol==videoCols)
	{
		cursCol = 0;
		cursRow++;
		if(cursRow==videoRows)
		{
			cursRow--;
			scrollUp(1);
			videoRamPtr -= videoCols;
		}
	}
} /* printCHAR */

/*-------------------------------------------------------------------------+
|         Function: printBS
|      Description: Print BS (BackSpace - '\b') character to display.
| Global Variables: videoRamPtr, cursRow, cursCol.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static inline void
printBS(void)
{
	/* Move cursor back one cell. */
	if(cursCol>0)
	{
		cursCol--;
	}
	else if(cursRow>0)
	{
		cursRow--;
		cursCol = videoCols - 1;
	}
	else
	{
		return;
	}

	/* Write a whitespace. */
	*(--videoRamPtr) = BLANK;
} /* printBS */


/*-------------------------------------------------------------------------+
|         Function: printHT
|      Description: Print HT (Horizontal Tab - '\t') character to display.
| Global Variables: cursCol.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static inline void
printHT(void)
{
	do
	{
		printCHAR(' ');
	}
	while (cursCol % TABSIZE);
} /* printHT */


/*-------------------------------------------------------------------------+
|         Function: printLF
|      Description: Print LF (Line Feed  - '\n') character to display.
| Global Variables: cursRow.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static inline void
printLF(void)
{
	cursRow++;
	if(cursRow==videoRows)
	{
		cursRow--;
		scrollUp(1);
	}
	updateVideoRamPtr();
} /* printLF */


/*-------------------------------------------------------------------------+
|         Function: printCR
|      Description: Print CR (Carriage Return - '\r') to display.
| Global Variables: cursCol.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static inline void
printCR(void)
{
	cursCol = 0;
	updateVideoRamPtr();
} /* printCR */

/*
 *  Console Device Driver Entry Points
 */
void
vga_write(
	int   minor, 
	char cChar)
{
	switch (cChar)
	{
		case '\b':
			printBS();
			break;
		case '\t':
			printHT();
			break;
		case '\n':
			printLF();
			break;
		case '\r':
			printCR();
			break;
		default:
			printCHAR(cChar);
			break;
	}

	setHardwareCursorPos(videoRamPtr - videoRam);
} /* vga_write */

/* 
 *  vga_write_support
 *
 *  Console Termios output entry point.
 *
 */
int vga_write_support(
	int   minor, 
	const char *buf, 
	int   len
)
{
	int nwrite = 0;

	while(bScrollLock)
	{
		/*
		 * The Scroll lock on the keyboard is active
		 */
		/*
		 * Yield while we wait
		 */
		rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
	}

	/*
	 * Write each byte in the string to the display
	 */
	while (nwrite<len)
	{
		/*
		 * transmit character
		 */
		vga_write(minor, *buf++);
		nwrite++;
	}

	/*
	 * return the number of bytes written.
	 */
	return nwrite;
}

boolean vga_probe(int minor)
{
	unsigned8 ucMiscIn;

	/*
	 * Check for presence of VGA adaptor
	 */
	inport_byte(0x3cc, ucMiscIn);
	if(ucMiscIn!=0xff)
	{
		/*
		 * VGA device is present
		 */
		return(TRUE);
	}
	return(FALSE);
}

void vga_init(int minor)
{
	scrollUp(videoRows);     /* Clear entire screen         */
	setHardwareCursorPos(0); /* Cursor at upper left corner */
	/*
	 * Enable the cursor
	 */
	VGA_WRITE_CRTC(0x0a, 0x0e);	/* Crt cursor start */
}
