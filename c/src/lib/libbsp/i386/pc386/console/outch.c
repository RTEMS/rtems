/*-------------------------------------------------------------------------+
| outch.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
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
| * All rights assigned to U.S. Government, 1994.                          *
| *                                                                        *
| * This material may be reproduced by or for the U.S. Government pursuant *
| * to the copyright license under the clause at DFARS 252.227-7013.  This *
| * notice must appear in all copies of this file and its derivatives.     *
| **************************************************************************
+--------------------------------------------------------------------------*/


#include <bsp.h>

#include <stdlib.h>
#include <string.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define DISPLAY_CELL_COUNT (MAX_ROW * MAX_COL)
                                       /* Number of display cells.            */
#define TABSIZE 4                      /* Number of spaces for TAB (\t) char. */
#define	WHITE	0x0700                 /* White on Black background colour.   */
#define BLANK   (WHITE | ' ')          /* Blank character.                    */


/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
static rtems_unsigned16 *videoRam    = TVRAM;
                           /* Physical address of start of video text memory. */
static rtems_unsigned16 *videoRamPtr = TVRAM;
                           /* Pointer for current output position in display. */
static rtems_unsigned8  videoRows = MAX_ROW; /* Number of rows in display.    */
static rtems_unsigned8  videoCols = MAX_COL; /* Number of columns in display. */
static rtems_unsigned8  cursRow   = 0;       /* Current cursor row.           */
static rtems_unsigned8  cursCol   = 0;       /* Current cursor column.        */


/*-------------------------------------------------------------------------+
|         Function: setHardwareCursorPos
|      Description: Set hardware video cursor at given offset into video RAM. 
| Global Variables: None.
|        Arguments: videoCursor - Offset into video memory.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static inline void
setHardwareCursorPos(rtems_unsigned16 videoCursor)
{
  outport_byte(GDC_REG_PORT, 0xe);
  outport_byte(GDC_VAL_PORT, (videoCursor >> 8) & 0xff);
  outport_byte(GDC_REG_PORT, 0xf);
  outport_byte(GDC_VAL_PORT, videoCursor & 0xff);
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
scrollUp(rtems_unsigned8 lines)
{
  rtems_unsigned16 blankCount;
                        /* Number of blank display cells on bottom of window. */
  rtems_unsigned16 *ptrDst, *ptrSrc;
               /* Source and destination pointers for memory copy operations. */

  if (lines < videoRows)  /* Move window's contents up. */
  {
    rtems_unsigned16 nonBlankCount;
       /* Number of non-blank cells on upper part of display (total - blank). */

    blankCount = lines * videoCols;
    nonBlankCount = DISPLAY_CELL_COUNT - blankCount;
    ptrSrc = videoRam + blankCount;
    ptrDst = videoRam; 

    while(nonBlankCount--)
      *ptrDst++ = *ptrSrc++;
  }
  else                    /* Clear the whole display.   */
  {
    blankCount = DISPLAY_CELL_COUNT;
    ptrDst = videoRam;
  }

  /* Fill bottom with blanks. */
  while (blankCount-- > 0)
    *ptrDst++ = BLANK;
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
  *videoRamPtr++ = c | WHITE;
  cursCol++;
  if (cursCol == videoCols)
  {
    cursCol = 0;
    cursRow++;
    if (cursRow == videoRows)
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
  if (cursCol > 0)
    cursCol--;
  else if (cursRow > 0)
  {
    cursRow--;
    cursCol = videoCols - 1;
  }
  else
    return;

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
    printCHAR(' ');
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
  if (cursRow == videoRows)
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


/*-------------------------------------------------------------------------+
|         Function: consPutc
|      Description: Print a character to display at current position.
| Global Variables: videoRamPtr, videoRam.
|        Arguments: c - character to write to display.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static void
consPutc(char c)
{
  switch (c)
  {
    case '\b': printBS();    break;
    case '\t': printHT();    break;
    case '\n': printLF();    break;
    case '\r': printCR();    break;
    default:   printCHAR(c); break;
  } /* switch */

  setHardwareCursorPos(videoRamPtr - videoRam);
                                           /* At current offset into videoRam */
} /* consPutc */


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
  consPutc(c);
} /* _IBMPC_outch */


/*-------------------------------------------------------------------------+
|         Function: _IBMPC_initVideo
|      Description: Video system initialization. Hook for any early setup.
| Global Variables: videoRows.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
_IBMPC_initVideo(void)
{
  scrollUp(videoRows);     /* Clear entire screen         */
  setHardwareCursorPos(0); /* Cursor at upper left corner */
} /* _IBMPC_initVideo */
