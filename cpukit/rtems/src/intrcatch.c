/**
 *  @file
 *
 *  @brief RTEMS Interrupt Catch
 *  @ingroup ClassicINTR
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/intr.h>
#include <rtems/score/isr.h>

#if (CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE)
rtems_status_code rtems_interrupt_catch(
  rtems_isr_entry      new_isr_handler,
  rtems_vector_number  vector,
  rtems_isr_entry     *old_isr_handler
)
{
  if ( vector > CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER )
    return RTEMS_INVALID_NUMBER;

  if ( new_isr_handler == NULL )
    return RTEMS_INVALID_ADDRESS;

  if ( old_isr_handler == NULL )
    return RTEMS_INVALID_ADDRESS;

  _ISR_Install_vector( vector, new_isr_handler, old_isr_handler );

  return RTEMS_SUCCESSFUL;
}
#endif
