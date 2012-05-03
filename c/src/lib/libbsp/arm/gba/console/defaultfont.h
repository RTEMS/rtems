/**
 *  @file defaultfont.c
 *
 *  This file contains default font definitions.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/** Font array dot value   */
#define  X  255
/** Generate font row data */
#define GEN_FONT_DATA(a,b,c,d)  ( ((d)<<24) | ((c)<<16) | ((b)<<8) | ((a)<<0) )
/** array for 3x5 font, font screen size is 4x6 */
static const unsigned long	font3x5[256][6] =
	{	{		/* ' ' - ascii:0x20 font:0x00   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* '!' - ascii:0x21 font:0x01   */
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* '"' - ascii:0x22 font:0x02   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* '#' - ascii:0x23 font:0x03   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* '$' - ascii:0x24 font:0x04    */
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* '%' - ascii:0x25 font:0x05   */
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,0  )
		},{		/* '&' - ascii:0x26 font:0x06   */
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* ''' - ascii:0x27 font:0x07   */
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* '(' - ascii:0x28 font:0x08   */
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,X  )
		},{		/* ')' - ascii:0x29 font:0x09   */
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  )
		},{		/* '*' - ascii:0x2A font:0x0A   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* '+' - ascii:0x2B font:0x0B   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* '´' - ascii:0x2C font:0x0C   */
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* '-' - ascii:0x2D font:0x0D   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* '.' - ascii:0x2E font:0x0E   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		},{		/* '/' - ascii:0x2F font:0x0F   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  X,0,0,0  )
		},{		/* '0' - ascii:0x30 font:0x10   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '1' - ascii:0x31 font:0x11   */
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '2' - ascii:0x32 font:0x12   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '3' - ascii:0x33 font:0x13   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '4' - ascii:0x34 font:0x14   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,X  )
		},{		/* '5' - ascii:0x35 font:0x15   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '6' - ascii:0x36 font:0x16   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '7' - ascii:0x37 font:0x17   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* '8' - ascii:0x38 font:0x18   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '9' - ascii:0x39 font:0x19   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* ':' - ascii:0x3A font:0x1A   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		},{		/* ';' - ascii:0x3B font:0x1B   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		},{		/* '<' - ascii:0x3C font:0x1C   */
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,X  )
		},{		/* '=' - ascii:0x3D font:0x1D   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* '>' - ascii:0x3E font:0x1E   */
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  )
		},{		/* '?' - ascii:0x3F font:0x1F   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* '@' - ascii:0x40 font:0x20   */
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,X  )
		},{		/* 'A' - ascii:0x41 font:0x21   */
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'B' - ascii:0x42 font:0x22   */
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  )
		},{		/* 'C' - ascii:0x43 font:0x23   */
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,X  )
		},{		/* 'D' - ascii:0x44 font:0x24   */
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		},{		/* 'E' - ascii:0x45 font:0x25   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* 'F' - ascii:0x46 font:0x26   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		},{		/* 'G' - ascii:0x47 font:0x27   */
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,X  )
		},{		/* 'H' - ascii:0x48 font:0x28   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'I' - ascii:0x49 font:0x29   */
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* 'J' - ascii:0x4A font:0x2A   */
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* 'K' - ascii:0x4B font:0x2B   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'L' - ascii:0x4C font:0x2C   */
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* 'M' - ascii:0x4D font:0x2D   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'N' - ascii:0x4E font:0x2E   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'O' - ascii:0x4F font:0x2F   */
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* 'P' - ascii:0x50 font:0x30   */
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		},{		/* 'Q' - ascii:0x51 font:0x31   */
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,X  )
		},{		/* 'R' - ascii:0x52 font:0x33   */
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'S' - ascii:0x53 font:0x33   */
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  )
		},{		/* 'T' - ascii:0x54 font:0x34   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		},{		/* 'U' - ascii:0x55 font:0x35   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* 'V' - ascii:0x56 font:0x36   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		},{		/* 'W' - ascii:0x57 font:0x37   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		},{		/* 'X' - ascii:0x58 font:0x38   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'Y' - ascii:0x59 font:0x39   */
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		},{		/* 'Z' - ascii:0x5A font:0x3A   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '[' - ascii:0x5B font:0x3B   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '\' - ascii:0x5C font:0x3C   */
		GEN_FONT_DATA(  X,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* ']' - ascii:0x5D font:0x3D   */
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '^' - ascii:0x5E font:0x3E   */
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* '_' - ascii:0x5F font:0x3F   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '´' - ascii:0x60 font:0x30   */
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* 'a' - ascii:0x31 font:0x31   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* 'b' - ascii:0x62 font:0x32   */
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  )
		},{		/* 'c' - ascii:0x63 font:0x33   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,X  )
		},{		/* 'd' - ascii:0x64 font:0x34   */
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		},{		/* 'e' - ascii:0x65 font:0x35   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,0,X,X  )
		},{		/* 'f' - ascii:0x66 font:0x36   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		},{		/* 'g' - ascii:0x67 font:0x37   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,X,0  )
		},{		/* 'h' - ascii:0x68 font:0x38   */
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'i' - ascii:0x69 font:0x39   */
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* 'j' - ascii:0x6A font:0x3A   */
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* 'k' - ascii:0x6B font:0x3B   */
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'l' - ascii:0x6C font:0x3C   */
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* 'm' - ascii:0x6D font:0x3D   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'n' - ascii:0x6E font:0x3E   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'o' - ascii:0x6F font:0x6F   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  )
		},{		/* 'p' - ascii:0x70 font:0x40   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		},{		/* 'q' - ascii:0x71 font:0x41   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  )
		},{		/* 'r' - ascii:0x72 font:0x43   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  )
		},{		/* 's' - ascii:0x73 font:0x43   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,X,X,0  )
		},{		/* 't' - ascii:0x74 font:0x44   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,X  ),
		},{		/* 'u' - ascii:0x75 font:0x45   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,X  )
		},{		/* 'v' - ascii:0x76 font:0x46   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		},{		/* 'w' - ascii:0x77 font:0x47   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		},{		/* 'x' - ascii:0x78 font:0x48   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,X  )
		},{		/* 'y' - ascii:0x79 font:0x49   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		},{		/* 'z' - ascii:0x7A font:0x4A   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,X,X  )
		},{		/* '{' - ascii:0x7B font:0x4B   */
		GEN_FONT_DATA(  0,0,X,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,X,X  )
		},{		/* '\' - ascii:0x7C font:0x4C   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* '}' - ascii:0x7D font:0x4D   */
		GEN_FONT_DATA(  0,X,X,0  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,X,0  ),
		GEN_FONT_DATA(  0,X,X,0  )
		},{		/* '~' - ascii:0x7E font:0x4E   */
		GEN_FONT_DATA(  0,0,0,0  ),
		GEN_FONT_DATA(  0,X,0,0  ),
		GEN_FONT_DATA(  0,X,X,X  ),
		GEN_FONT_DATA(  0,0,0,X  ),
		GEN_FONT_DATA(  0,0,0,0  )
		},{		/* DEL - ascii:0x7F font:0x4F   */
		GEN_FONT_DATA(  X,X,X,X  ),
		GEN_FONT_DATA(  X,X,X,X  ),
		GEN_FONT_DATA(  X,X,X,X  ),
		GEN_FONT_DATA(  X,X,X,X  ),
		GEN_FONT_DATA(  X,X,X,X  )
		}
	};
