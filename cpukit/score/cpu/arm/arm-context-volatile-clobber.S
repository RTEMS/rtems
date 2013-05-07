/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>

	.section	.text

FUNCTION_THUMB_ENTRY(_CPU_Context_volatile_clobber)

.macro clobber_register reg
	sub	r0, r0, #1
	mov	\reg, r0
.endm

	clobber_register	r1
	clobber_register	r2
	clobber_register	r3
	clobber_register	r12

	bx	lr

FUNCTION_END(_CPU_Context_volatile_clobber)
