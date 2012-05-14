/*
 *  Support for SuperH Simulator in GDB
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/system.h>
#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/sh.h>

#if (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  unsigned long    *_old_stack_ptr;
#endif

register unsigned long  *stack_ptr __asm__ ("r15");

/*
 *  This routine provides the RTEMS interrupt management.
 */

void __ISR_Handler( uint32_t   vector)
{
  ISR_Level level;

  _ISR_Disable( level );

  _Thread_Dispatch_increment_disable_level();

#if (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  if ( _ISR_Nest_level == 0 )
    {
      /* Install irq stack */
      _old_stack_ptr = stack_ptr;
      stack_ptr = _CPU_Interrupt_stack_high;
    }

#endif

  _ISR_Nest_level++;

  _ISR_Enable( level );

  /* call isp */
  if ( _ISR_Vector_table[ vector])
    (*_ISR_Vector_table[ vector ])( vector );

  _ISR_Disable( level );

  _Thread_Dispatch_decrement_disable_level();

  _ISR_Nest_level--;

#if (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)

  if ( _ISR_Nest_level == 0 )
    /* restore old stack pointer */
    stack_ptr = _old_stack_ptr;
#endif

  _ISR_Enable( level );

  if ( _ISR_Nest_level )
    return;

  if ( _Thread_Dispatch_in_critical_section() ) {
    return;
  }

  if ( _Thread_Dispatch_necessary ) {
    _Thread_Dispatch();
  }
}
