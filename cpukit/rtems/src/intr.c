/*
 *  Interrupt Manager
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/intr.h>

/*  _Interrupt_Manager_initialization
 *
 *  This routine initializes the interrupt manager.
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 */

void _Interrupt_Manager_initialization( void )
{
}

/*  rtems_interrupt_catch
 *
 *  This directive allows a thread to specify what action to take when
 *  catching signals.
 *
 *  Input parameters:
 *    new_isr_handler - address of interrupt service routine (isr)
 *    vector          - interrupt vector number
 *    old_isr_handler - address at which to store previous ISR address
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - always succeeds
 *    *old_isr_handler  - previous ISR address
 */

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

  _ISR_Install_vector(
    vector, (proc_ptr)new_isr_handler, (proc_ptr *)old_isr_handler );

  return RTEMS_SUCCESSFUL;
}
