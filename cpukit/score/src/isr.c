/**
 *  @file
 *
 *  @brief Initialize the ISR handler
 *  @ingroup ScoreISR
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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
#include <rtems/score/address.h>
#include <rtems/score/isr.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/wkspace.h>
#include <rtems/config.h>

void _ISR_Handler_initialization( void )
{
  _ISR_Nest_level = 0;

#if (CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE)
  _ISR_Vector_table = _Workspace_Allocate_or_fatal_error(
     sizeof(ISR_Handler_entry) * ISR_NUMBER_OF_VECTORS
  );

  _CPU_Initialize_vectors();
#endif

#if ( CPU_ALLOCATE_INTERRUPT_STACK == TRUE )
  {
    size_t stack_size = rtems_configuration_get_interrupt_stack_size();

    if ( !_Stack_Is_enough( stack_size ) )
      _Internal_error_Occurred(
        INTERNAL_ERROR_CORE,
        true,
        INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL
      );

    _CPU_Interrupt_stack_low = _Workspace_Allocate_or_fatal_error(
      stack_size
    );

    _CPU_Interrupt_stack_high = _Addresses_Add_offset(
      _CPU_Interrupt_stack_low,
      stack_size
    );
  }

#if (CPU_STACK_ALIGNMENT != 0)
  _CPU_Interrupt_stack_high = (void *)
    ((uintptr_t) _CPU_Interrupt_stack_high & ~(CPU_STACK_ALIGNMENT - 1));
#endif

  /* Interrupt stack might have to be aligned and/or setup
   * in a specific way.
   */
#if defined(_CPU_Interrupt_stack_setup)
  _CPU_Interrupt_stack_setup(_CPU_Interrupt_stack_low, _CPU_Interrupt_stack_high);
#endif

#endif

#if ( CPU_HAS_HARDWARE_INTERRUPT_STACK == TRUE )
  _CPU_Install_interrupt_stack();
#endif

#if defined(RTEMS_SMP)
  _ISR_SMP_Initialize();
#endif
}
