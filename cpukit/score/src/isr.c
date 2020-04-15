/**
 *  @file
 *
 *  @brief Initialize the ISR handler
 *  @ingroup RTEMSScoreISR
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/isr.h>
#include <rtems/score/address.h>
#include <rtems/score/interr.h>
#include <rtems/score/percpu.h>
#include <rtems/score/stackimpl.h>
#include <rtems/config.h>

#if (CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE)
  ISR_Handler_entry _ISR_Vector_table[ CPU_INTERRUPT_NUMBER_OF_VECTORS ];
#elif defined(CPU_INTERRUPT_NUMBER_OF_VECTORS)
  #error "CPU_INTERRUPT_NUMBER_OF_VECTORS is defined for non-simple vectored interrupts"
#elif defined(CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER)
  #error "CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER is defined for non-simple vectored interrupts"
#endif

void _ISR_Handler_initialization( void )
{
  uint32_t  cpu_max;
  uint32_t  cpu_index;
  size_t    stack_size;
  char     *stack_low;

  _ISR_Nest_level = 0;

#if (CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE)
  _CPU_Initialize_vectors();
#endif

  stack_size = rtems_configuration_get_interrupt_stack_size();
  cpu_max = rtems_configuration_get_maximum_processors();
  stack_low = _ISR_Stack_area_begin;

  for ( cpu_index = 0 ; cpu_index < cpu_max; ++cpu_index ) {
    Per_CPU_Control *cpu;
    char            *stack_high;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    stack_high = _Addresses_Add_offset( stack_low, stack_size );

    cpu->interrupt_stack_low = stack_low;
    cpu->interrupt_stack_high = stack_high;

    /*
     * Interrupt stack might have to be aligned and/or setup in a specific
     * way.  Do not use the local low or high variables here since
     * _CPU_Interrupt_stack_setup() is a nasty macro that might want to play
     * with the real memory locations.
     */
#if defined(_CPU_Interrupt_stack_setup)
    _CPU_Interrupt_stack_setup(
      cpu->interrupt_stack_low,
      cpu->interrupt_stack_high
    );
#endif

    stack_low = stack_high;
  }
}
