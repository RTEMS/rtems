/*
 * crt.h  - This file contains definitions for constants related to PC console.
 *          More information can be found at
 *	<http://millenium.atcg.com/~taco/helppc/tables.html>
 *
 * Copyright (C) 1998  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _CRT_H
#define _CRT_H

/*
 * Bitmap video origins in text mode.
 */

	/* In monochrome */
#define V_MONO	0xb0000
	/* In color	 */
#define V_COLOR	0xb8000

/*
 * Video Option Byte location. It must be maintained
 * by the BIOS.
 */
#define VIDEO_MODE_ADDR          	0x449
/*
 * Video controller base IO address location in
 * BIOS data area
 */
#define DISPLAY_CRT_BASE_IO_ADDR  	0x463
/*
 * Number of collums and lines locations for the
 * actual video Configuration
 */
#define NB_MAX_COL_ADDR			0x44a
#define NB_MAX_ROW_ADDR			0x484

    /*
     * Miscellaneous information set by the BIOS in offset video_mode,
     */

#   define VGAMODE7	0x7	/* VGA mode 7 */

    /* Color codes in text mode for background and foreground. */
#define BLACK		0x0
#define BLUE		0x1
#define GREEN		0x2
#define CYAN		0x3
#define RED	   	0x4
#define MAGENTA		0x5
#define BROWN		0x6
#define WHITE		0x7

#define GRAY		0x8
#define LT_BLUE		0x9
#define LT_GREEN	0xa
#define LT_CYAN		0xb
#define LT_RED		0xc
#define LT_MAGENTA	0xd
#define YELLOW		0xe
#define LT_WHITE	0xf

#define BLINK           0x8	/* Mask used to determine blinking */
#define OFF             0
#define ON              1

/*
 * CRT Controller register offset definitions
 */

#   define CC_CURSSTART	0x0a	/* Cursor start scan line */
#   define CC_CURSEND	0x0b	/* Cursor end scan line */
#   define CC_STARTADDRHI 0x0c	/* start video ram addr msb */
#   define CC_STARTADDRLO 0x0d	/* start video ram lsb */
#   define CC_CURSHIGH	0x0e	/* Cursor high location (8 msb)	*/
#   define CC_CURSLOW	0x0f	/* Cursor low location (8 msb) */
#   define CC_VRETSTART	0x10	/* vertical synchro start lsb */
#   define CC_VRETEND	0x11	/* vertical syn end (also it control) */
#   define CC_VDISPEND	0x12	/* vertical display end lsb register */

#endif /* _CRT_H */
