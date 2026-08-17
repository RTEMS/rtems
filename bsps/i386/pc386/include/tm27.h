/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief Implementation of interrupt mechanisms for Time Test 27.
 */

/*
 *  tm27.h
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <bsp/irq.h>

/*
 *  Define the interrupt mechanism for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define TM27_INTERRUPT_VECTOR_DEFAULT BSP_SOFTWARE_IRQ

static rtems_interrupt_entry pc386_tm27_interrupt_entry;

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  rtems_interrupt_entry_initialize(
    &pc386_tm27_interrupt_entry,
    handler,
    NULL,
    "tm27"
  );
  (void) rtems_interrupt_entry_install(
    BSP_SOFTWARE_IRQ,
    RTEMS_INTERRUPT_SHARED,
    &pc386_tm27_interrupt_entry
  );
}

static inline void Cause_tm27_intr( void )
{
  (void) rtems_interrupt_raise( BSP_SOFTWARE_IRQ );
}

/*
 * The INT instruction leaves nothing pending, so there is nothing to clear.
 */
static inline void Clear_tm27_intr( void )
{
}

static inline void Lower_tm27_intr( void )
{
}

#endif
