/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file 
 * @brief This file contains code for displaying cursor on the console
 */

/*
 * Copyright (C) 1998  eric.valette@free.fr
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
