/*
 *  ISR Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/stack.h>
#include <rtems/score/interr.h>
#include <rtems/score/wkspace.h>

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

  _ISR_Vector_table = _Workspace_Allocate_or_fatal_error(
     sizeof(ISR_Handler_entry) * ISR_NUMBER_OF_VECTORS
  );

  _CPU_Initialize_vectors();

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
