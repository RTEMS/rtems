/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/**
 *  @file
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/*
 *  Define the interrupt mechanism for Time Test 27
 */

#include <bsp/irq.h>
#include <rtems/score/isr.h>

#define MUST_WAIT_FOR_INTERRUPT 1

#define TM27_INTERRUPT_VECTOR_DEFAULT TX3904_IRQ_SOFTWARE_1

static rtems_interrupt_entry jmr3904_tm27_interrupt_entry;

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  rtems_interrupt_entry_initialize(
    &jmr3904_tm27_interrupt_entry,
    handler,
    NULL,
    "tm27"
  );
  (void) rtems_interrupt_entry_install(
    TM27_INTERRUPT_VECTOR_DEFAULT,
    RTEMS_INTERRUPT_SHARED,
    &jmr3904_tm27_interrupt_entry
  );
}

/*
 * The software interrupt of the processor is a bit of the cause register, so
 * it is pending from the write on and arrives as soon as the interrupts are
 * enabled.  It stays pending until it is cleared.
 */
static inline void Cause_tm27_intr( void )
{
  (void) rtems_interrupt_raise( TM27_INTERRUPT_VECTOR_DEFAULT );
}

static inline void Clear_tm27_intr( void )
{
  (void) rtems_interrupt_clear( TM27_INTERRUPT_VECTOR_DEFAULT );
}

static inline void Lower_tm27_intr( void )
{
  _ISR_Set_level( 0 );
}

#endif
