/*
 *  ISR Handler
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
#include <rtems/core/isr.h>
#include <rtems/core/stack.h>
#include <rtems/core/interr.h>
#include <rtems/core/wkspace.h>

/*  _ISR_Handler_initialization
 *
 *  This routine initializes the ISR handler.
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 */

void _ISR_Handler_initialization( void )
{
  _ISR_Signals_to_thread_executing = FALSE;

  _ISR_Nest_level = 0;

#if ( CPU_ALLOCATE_INTERRUPT_STACK == TRUE )

  if ( _CPU_Table.interrupt_stack_size < STACK_MINIMUM_SIZE )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL
    );

  _CPU_Interrupt_stack_low =
    _Workspace_Allocate_or_fatal_error( _CPU_Table.interrupt_stack_size );

  _CPU_Interrupt_stack_high = _Addresses_Add_offset(
    _CPU_Interrupt_stack_low,
    _CPU_Table.interrupt_stack_size
  );

#endif

#if ( CPU_HAS_HARDWARE_INTERRUPT_STACK == TRUE )
  _CPU_Install_interrupt_stack();
#endif

}
