/*
 *  NIOS2 exception and interrupt handler
 *
 *  Derived from c4x/irq.c
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>

/*
 *  This routine provides the RTEMS interrupt management.
 *
 *  Upon entry, interrupts are disabled
 */

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  unsigned long    *_old_stack_ptr;
#endif

register unsigned long  *stack_ptr asm("sp");

void __ISR_Handler(uint32_t vector, CPU_Interrupt_frame *ifr)
{
  register uint32_t   level;

  /* Interrupts are disabled upon entry to this Handler */

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  if ( _ISR_Nest_level == 0 ) {
    /* Install irq stack */
    _old_stack_ptr = stack_ptr;
    stack_ptr = _CPU_Interrupt_stack_high - 4;
  }
#endif

  _ISR_Nest_level++;

  _Thread_Dispatch_disable_level++;

  if ( _ISR_Vector_table[ vector] )
  {
    (*_ISR_Vector_table[ vector ])(vector, ifr);
  };

  /* Make sure that interrupts are disabled again */
  _CPU_ISR_Disable( level );

  _Thread_Dispatch_disable_level--;

  _ISR_Nest_level--;

  if( _ISR_Nest_level == 0)
  {
#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
    stack_ptr = _old_stack_ptr;
#endif

    if( _Thread_Dispatch_disable_level == 0 )
    {
      if ( _Context_Switch_necessary || _ISR_Signals_to_thread_executing )
      {
        _ISR_Signals_to_thread_executing = FALSE;
        _CPU_ISR_Enable( level );
        _Thread_Dispatch();
        /* may have switched to another task and not return here immed. */
        _CPU_ISR_Disable( level ); /* Keep _pairs_ of Enable/Disable */
      }
    }
    else
    {
      _ISR_Signals_to_thread_executing = FALSE;
    };
  };

  _CPU_ISR_Enable( level );
}

void __Exception_Handler(CPU_Exception_frame *efr)
{
  _CPU_Fatal_halt(0xECC0);
}


