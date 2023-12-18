/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup RTEMSBSPsSPARCLEON3
 * @brief Implementations for interrupt mechanisms for Time Test 27
 */

/*
 *  COPYRIGHT (c) 2006.
 *  Aeroflex Gaisler AB.
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

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <bsp.h>
#include <bsp/irq.h>

#if defined(RTEMS_SMP)
#include <rtems/score/smpimpl.h>
#endif

/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Since the interrupt code for the SPARC supports both synchronous
 *        and asynchronous trap handlers, support for testing with both
 *        is included.
 */

#define SIS_USE_SYNCHRONOUS_TRAP  0

/*
 *  The synchronous trap is an arbitrarily chosen software trap.
 */

#if (SIS_USE_SYNCHRONOUS_TRAP == 1)

#define TEST_VECTOR SPARC_SYNCHRONOUS_TRAP( 0x90 )

#define MUST_WAIT_FOR_INTERRUPT 1

#define TM27_USE_VECTOR_HANDLER

#define Install_tm27_vector( handler ) \
  set_vector( (handler), TEST_VECTOR, 1 );

#define Cause_tm27_intr() \
  __asm__ volatile( "ta 0x10; nop " );

#define Clear_tm27_intr() /* empty */

#define Lower_tm27_intr() /* empty */

/*
 *  The asynchronous trap is an arbitrarily chosen ERC32 interrupt source.
 */

#else   /* use a regular asynchronous trap */

extern uint32_t Interrupt_nest;

#define TEST_INTERRUPT_SOURCE 5
#define TEST_INTERRUPT_SOURCE2 6
#define MUST_WAIT_FOR_INTERRUPT 1
#define TM27_INTERRUPT_VECTOR_DEFAULT TEST_INTERRUPT_SOURCE

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  static rtems_interrupt_entry entry_low;
  static rtems_interrupt_entry entry_high;

#if defined(RTEMS_SMP)
  bsp_interrupt_set_affinity(
    TEST_INTERRUPT_SOURCE,
    _SMP_Get_online_processors()
  );
  bsp_interrupt_set_affinity(
    TEST_INTERRUPT_SOURCE2,
    _SMP_Get_online_processors()
  );
#endif

  rtems_interrupt_entry_initialize(
    &entry_low,
    handler,
    NULL,
    "tm27 low"
  );
  (void) rtems_interrupt_entry_install(
    TEST_INTERRUPT_SOURCE,
    RTEMS_INTERRUPT_SHARED,
    &entry_low
  );
  rtems_interrupt_entry_initialize(
    &entry_high,
    handler,
    NULL,
    "tm27 high"
  );
  (void) rtems_interrupt_entry_install(
    TEST_INTERRUPT_SOURCE2,
    RTEMS_INTERRUPT_SHARED,
    &entry_high
  );
}

static inline void Cause_tm27_intr( void )
{
  rtems_vector_number vector;

  vector = TEST_INTERRUPT_SOURCE + ( Interrupt_nest >> 1 );
#if defined(RTEMS_SMP)
  (void) rtems_interrupt_raise_on( vector, rtems_scheduler_get_processor() );
#else
  (void) rtems_interrupt_raise( vector );
#endif
  nop();
  nop();
  nop();
}

static inline void Clear_tm27_intr( void )
{
  (void) rtems_interrupt_clear( TEST_INTERRUPT_SOURCE );
}

#define Lower_tm27_intr() /* empty */

#endif

#endif
