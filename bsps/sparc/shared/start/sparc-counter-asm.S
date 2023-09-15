/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2016, 2023 embedded brains GmbH & Co. KG
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

	/*
	 * This is a workaround for:
	 * https://gcc.gnu.org/bugzilla//show_bug.cgi?id=69027
	 */
	PUBLIC(_CPU_Counter_read)
SYM(_CPU_Counter_read):
	sethi	%hi(_SPARC_Counter + 4), %o1
	ld	[%o1 + %lo(_SPARC_Counter + 4)], %o1
	or	%o7, %g0, %g1
	call	%o1, 0
	 or	%g1, %g0, %o7

#if defined(RTEMS_PROFILING)
	/*
	 * This is a workaround for:
	 * https://gcc.gnu.org/bugzilla//show_bug.cgi?id=69027
	 */
	PUBLIC(_SPARC_Counter_read_ISR_disabled)
SYM(_SPARC_Counter_read_ISR_disabled):
	sethi	%hi(_SPARC_Counter), %o1
	ld	[%o1 + %lo(_SPARC_Counter)], %o1
	or	%o7, %g0, %g1
	call	%o1, 0
	 or	%g1, %g0, %o7
#endif

	PUBLIC(_SPARC_Counter_read_default)
SYM(_SPARC_Counter_read_default):
	sethi	%hi(_SPARC_Counter + 12), %o1
	ld	[%o1 + %lo(_SPARC_Counter + 12)], %o0
	add	%o0, 1, %o0
	st	%o0, [%o1 + %lo(_SPARC_Counter + 12)]
	jmp	%o7 + 8
	 nop

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
#ifdef __FIX_LEON3FT_TN0018
	/* A nop is added to work around the GRLIB-TN-0018 errata */
	nop
#endif
	jmp	%o7 + 8
	 sub	%o4, %o0, %o0
.Lpending:
	ld	[%o5 + 24], %o5
	ld	[%o3], %o0
	ta	SPARC_SWTRAP_IRQEN
	add	%o4, %o5, %o4
	jmp	%o7 + 8
	 sub	%o4, %o0, %o0
