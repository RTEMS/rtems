/*
 * Copyright (c) 2016, 2018 embedded brains GmbH.  All rights reserved.
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

	/*
	 * All functions except _SPARC_Counter_read_clock() in this module are
	 * sometimes called with traps disabled.
	 */

	.section	".text"
	.align	4

	PUBLIC(_SPARC_Counter_read_default)
SYM(_SPARC_Counter_read_default):
	sethi	%hi(_SPARC_Counter + 12), %o1
	ld	[%o1 + %lo(_SPARC_Counter + 12)], %o0
	add	%o0, 1, %o0
	jmp	%o7 + 8
	 st	%o0, [%o1 + %lo(_SPARC_Counter + 12)]

	PUBLIC(_SPARC_Counter_read_up)
	PUBLIC(_SPARC_Get_timecount_up)
SYM(_SPARC_Counter_read_up):
SYM(_SPARC_Get_timecount_up):
	sethi	%hi(_SPARC_Counter + 8), %o0
	ld	[%o0 + %lo(_SPARC_Counter + 8)], %o0
	jmp	%o7 + 8
	 ld	[%o0], %o0

	PUBLIC(_SPARC_Counter_read_down)
	PUBLIC(_SPARC_Get_timecount_down)
SYM(_SPARC_Counter_read_down):
SYM(_SPARC_Get_timecount_down):
	sethi	%hi(_SPARC_Counter + 8), %o0
	ld	[%o0 + %lo(_SPARC_Counter + 8)], %o0
	ld	[%o0], %o0
	jmp	%o7 + 8
	 xnor	%g0, %o0, %o0

	/*
	 * For the corresponding C code is something like this:
	 *
	 * CPU_Counter_ticks _SPARC_Counter_read_clock_isr_disabled( void )
	 * {
	 *   const SPARC_Counter *ctr;
	 *   CPU_Counter_ticks    ticks;
	 *   CPU_Counter_ticks    accumulated;
	 *
	 *   ctr = &_SPARC_Counter;
	 *   ticks = *ctr->counter_register;
	 *   accumulated = ctr->accumulated;
	 *
	 *   if ( ( *ctr->pending_register & ctr->pending_mask ) != 0 ) {
	 *     ticks = *ctr->counter_register;
	 *     accumulated += ctr->interval;
	 *   }
	 *
	 *   return accumulated - ticks;
	 * }
	 */
	PUBLIC(_SPARC_Counter_read_clock_isr_disabled)
SYM(_SPARC_Counter_read_clock_isr_disabled):
	sethi	%hi(_SPARC_Counter), %o5
	or	%o5, %lo(_SPARC_Counter), %o5
	ld	[%o5 + 8], %o3
	ld	[%o5 + 12], %o4
	ld	[%o5 + 16], %o2
	ld	[%o3], %o0
	ld	[%o4], %o1
	btst	%o1, %o2
	bne	.Lpending_isr_disabled
	 ld	[%o5 + 20], %o4
	jmp	%o7 + 8
	 sub	%o4, %o0, %o0
.Lpending_isr_disabled:
	ld	[%o5 + 24], %o5
	ld	[%o3], %o0
	add	%o4, %o5, %o4
	jmp	%o7 + 8
	 sub	%o4, %o0, %o0

	/*
	 * For the corresponding C code see
	 * _SPARC_Counter_read_clock_isr_disabled() above.
	 */
	PUBLIC(_SPARC_Counter_read_clock)
	PUBLIC(_SPARC_Get_timecount_clock)
SYM(_SPARC_Counter_read_clock):
SYM(_SPARC_Get_timecount_clock):
	sethi	%hi(_SPARC_Counter), %o5
	or	%o5, %lo(_SPARC_Counter), %o5
	ta	SPARC_SWTRAP_IRQDIS
	ld	[%o5 + 8], %o3
	ld	[%o5 + 12], %o4
	ld	[%o5 + 16], %o2
	ld	[%o3], %o0
	ld	[%o4], %o1
	btst	%o1, %o2
	bne	.Lpending
	 ld	[%o5 + 20], %o4
	ta	SPARC_SWTRAP_IRQEN
	jmp	%o7 + 8
	 sub	%o4, %o0, %o0
.Lpending:
	ld	[%o5 + 24], %o5
	ld	[%o3], %o0
	ta	SPARC_SWTRAP_IRQEN
	add	%o4, %o5, %o4
	jmp	%o7 + 8
	 sub	%o4, %o0, %o0

	PUBLIC(_SPARC_Counter_read_asr23)
	PUBLIC(_SPARC_Get_timecount_asr23)
SYM(_SPARC_Counter_read_asr23):
SYM(_SPARC_Get_timecount_asr23):
	jmp	%o7 + 8
	 mov	%asr23, %o0
