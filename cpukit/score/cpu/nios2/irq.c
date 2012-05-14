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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/nios2-utility.h>

/*
 *  This routine provides the RTEMS interrupt management.
 *
 *  Upon entry, interrupts are disabled
 */

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  unsigned long    *_old_stack_ptr;
#endif

register unsigned long  *stack_ptr __asm__ ("sp");

RTEMS_INLINE_ROUTINE void __IIC_Handler(void)
{
  uint32_t active;
  uint32_t mask;
  uint32_t vector;

  /*
   * Obtain from the interrupt controller a bit list of pending interrupts,
   * and then process the highest priority interrupt. This process loops,
   * loading the active interrupt list on each pass until ipending
   * return zero.
   *
   * The maximum interrupt latency for the highest priority interrupt is
   * reduced by finding out which interrupts are pending as late as possible.
   * Consider the case where the high priority interupt is asserted during
   * the interrupt entry sequence for a lower priority interrupt to see why
   * this is the case.
   */

  active = _Nios2_Get_ctlreg_ipending();

  while (active)
  {
    vector = 0;
    mask = 1;

    /*
     * Test each bit in turn looking for an active interrupt. Once one is
     * found call it to clear the interrupt condition.
     */

    while (active)
    {
      if (active & mask)
      {
        if ( _ISR_Vector_table[ vector] )
          (*_ISR_Vector_table[ vector ])(vector, NULL);
        active &= ~mask;
      }
      mask <<= 1;
      ++vector;
    };

    active = _Nios2_Get_ctlreg_ipending();
  }
  
}

void __ISR_Handler(void)
{
  register uint32_t level;

  /* Interrupts are disabled upon entry to this Handler */

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  if ( _ISR_Nest_level == 0 ) {
    /* Install irq stack */
    _old_stack_ptr = stack_ptr;
    stack_ptr = _CPU_Interrupt_stack_high - 4;
  }
#endif

  _ISR_Nest_level++;

  _Thread_Dispatch_increment_disable_level();

  __IIC_Handler();
  
  /* Make sure that interrupts are disabled again */
  _CPU_ISR_Disable( level );

  _Thread_Dispatch_decrement_disable_level();

  _ISR_Nest_level--;

  if( _ISR_Nest_level == 0) {
#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
    stack_ptr = _old_stack_ptr;
#endif

    if( !_Thread_Dispatch_in_critical_section() )
    {
      if ( _Thread_Dispatch_necessary ) {
        _CPU_ISR_Enable( level );
        _Thread_Dispatch();
        /* may have switched to another task and not return here immed. */
        _CPU_ISR_Disable( level ); /* Keep _pairs_ of Enable/Disable */
      }
    }
  }

  _CPU_ISR_Enable( level );
}

void __Exception_Handler(CPU_Exception_frame *efr)
{
  _CPU_Fatal_halt(0xECC0);
}
