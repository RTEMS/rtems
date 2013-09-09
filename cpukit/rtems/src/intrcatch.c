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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/intr.h>

#if (CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE)

/**
 * This function returns true if the vector is a valid vector number
 * for this processor and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _ISR_Is_vector_number_valid (
  uint32_t   vector
)
{
  return ( vector <= CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER );
}

/**
 * This function returns true if handler is the entry point of a valid
 * use interrupt service routine and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _ISR_Is_valid_user_handler (
  void *handler
)
{
  return (handler != NULL);
}

rtems_status_code rtems_interrupt_catch(
  rtems_isr_entry      new_isr_handler,
  rtems_vector_number  vector,
  rtems_isr_entry     *old_isr_handler
)
{
  if ( !_ISR_Is_vector_number_valid( vector ) )
    return RTEMS_INVALID_NUMBER;

  if ( !_ISR_Is_valid_user_handler( (void *) new_isr_handler ) )
    return RTEMS_INVALID_ADDRESS;

  if ( !_ISR_Is_valid_user_handler( (void *) old_isr_handler ) )
    return RTEMS_INVALID_ADDRESS;

  _ISR_Install_vector(
    vector, (proc_ptr)new_isr_handler, (proc_ptr *)old_isr_handler );

  return RTEMS_SUCCESSFUL;
}
#endif
