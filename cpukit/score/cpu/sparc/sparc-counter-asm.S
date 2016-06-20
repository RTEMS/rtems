/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>

	.section	".text"
	.align	4

	PUBLIC(_SPARC_Counter_read_address)
SYM(_SPARC_Counter_read_address):
	sethi	%hi(_SPARC_Counter + 8), %o0
	ld	[%o0 + %lo(_SPARC_Counter + 8)], %o0
	jmp	%o7 + 8
	 ld	[%o0], %o0

	PUBLIC(_SPARC_Counter_read_asr23)
SYM(_SPARC_Counter_read_asr23):
	jmp	%o7 + 8
	 mov	%asr23, %o0
