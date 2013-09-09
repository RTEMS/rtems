/**
 *  @file
 *
 *  @brief LM32 Initialize the ISR Handler
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/isr.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threaddispatch.h>

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  unsigned long    *_old_stack_ptr;
#endif

void *_exception_stack_frame;

register unsigned long  *stack_ptr __asm__ ("sp");

void __ISR_Handler(uint32_t vector, CPU_Interrupt_frame *ifr)
{
  register uint32_t   level;
  _exception_stack_frame = NULL;

  /* Interrupts are disabled upon entry to this Handler */

  _Thread_Dispatch_increment_disable_level();

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  if ( _ISR_Nest_level == 0 ) {
    /* Install irq stack */
    _old_stack_ptr = stack_ptr;
    stack_ptr = _CPU_Interrupt_stack_high - 4;
  }
#endif

  _ISR_Nest_level++;

  if ( _ISR_Vector_table[ vector] )
  {
    (*_ISR_Vector_table[ vector ])(vector, ifr);
  };

  /* Make sure that interrupts are disabled again */
  _CPU_ISR_Disable( level );

  _ISR_Nest_level--;

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  if( _ISR_Nest_level == 0)
    stack_ptr = _old_stack_ptr;
#endif

  _Thread_Dispatch_decrement_disable_level();

  _CPU_ISR_Enable( level );

  if ( _ISR_Nest_level )
    return;

  if ( _Thread_Dispatch_necessary && _Thread_Dispatch_is_enabled() ) {
    /* save off our stack frame so the context switcher can get to it */
    _exception_stack_frame = ifr;

    _Thread_Dispatch();

    /* and make sure its clear in case we didn't dispatch. if we did, its
     * already cleared */
    _exception_stack_frame = NULL;
  }
}

