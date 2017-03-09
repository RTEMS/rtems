/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>

	.section	.text

#ifdef __thumb__
FUNCTION_THUMB_ENTRY(_CPU_Context_volatile_clobber)
#else
FUNCTION_ENTRY(_CPU_Context_volatile_clobber)
#endif

.macro clobber_register reg
	sub	r0, r0, #1
	mov	\reg, r0
.endm

#ifdef ARM_MULTILIB_VFP
	vmrs	r1, FPSCR
	ldr	r2, =0xf000001f
	bic	r1, r1, r2
	and	r2, r2, r0
	orr	r1, r1, r2
	vmsr	FPSCR, r1

.macro clobber_vfp_register reg
	sub	r0, r0, #1
	vmov	\reg, r0, r0
.endm

	clobber_vfp_register	d0
	clobber_vfp_register	d1
	clobber_vfp_register	d2
	clobber_vfp_register	d3
	clobber_vfp_register	d4
	clobber_vfp_register	d5
	clobber_vfp_register	d6
	clobber_vfp_register	d7
#ifdef ARM_MULTILIB_VFP_D32
	clobber_vfp_register	d16
	clobber_vfp_register	d17
	clobber_vfp_register	d18
	clobber_vfp_register	d19
	clobber_vfp_register	d20
	clobber_vfp_register	d21
	clobber_vfp_register	d22
	clobber_vfp_register	d23
	clobber_vfp_register	d24
	clobber_vfp_register	d25
	clobber_vfp_register	d26
	clobber_vfp_register	d27
	clobber_vfp_register	d28
	clobber_vfp_register	d29
	clobber_vfp_register	d30
	clobber_vfp_register	d31
#endif /* ARM_MULTILIB_VFP_D32 */
#endif /* ARM_MULTILIB_VFP */

	clobber_register	r1
	clobber_register	r2
	clobber_register	r3
	clobber_register	r12

	bx	lr

FUNCTION_END(_CPU_Context_volatile_clobber)
