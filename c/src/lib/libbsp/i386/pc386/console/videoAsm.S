/*
 * videoAsm.S  - This file contains code for displaying cursor on the console
 *
 * Copyright (C) 1998  valette@crf.canon.fr
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

	.file "videoAsm.s"

#include <crt.h>

	.text
	.align 4
    	.globl  wr_cursor	/* Move cursor position */

/*
 *   	void wr_cursor(newPosition, ioBaseAddr)
 */

wr_cursor:	pushl	%ecx
		movl	8(%esp), %ecx		/* Get new cursor position */
		movb	$CC_CURSHIGH, %al	/* Cursor high location */
		movl	12(%esp), %edx		/* Get IO base address */
		outb	(%dx)
		incw	%dx			/* Program Data register */
		movb	%ch, %al
		outb	(%dx)			/* Update high location cursor */
		decw	%dx			/* Program Index Register */
		movb	$CC_CURSLOW, %al	/* Cursor low location */
		outb	(%dx)
		incw	%dx			/* Program Data Register */
		movb	%cl, %al
		outb	(%dx)			/* Update low location cursor */
		popl	%ecx
		ret
