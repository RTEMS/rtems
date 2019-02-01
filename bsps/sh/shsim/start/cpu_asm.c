/*
 *  Support for SuperH Simulator in GDB
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/sh.h>

unsigned long  *_old_stack_ptr;

register unsigned long  *stack_ptr __asm__ ("r15");

void __ISR_Handler(uint32_t vector);

/*
 *  This routine provides the RTEMS interrupt management.
 */
void __ISR_Handler( uint32_t   vector)
{
  ISR_Level level;

  _ISR_Local_disable( level );

  _Thread_Dispatch_disable();

  if ( _ISR_Nest_level == 0 )
    {
      /* Install irq stack */
      _old_stack_ptr = stack_ptr;
      stack_ptr = _CPU_Interrupt_stack_high;
    }

  _ISR_Nest_level++;

  _ISR_Local_enable( level );

  /* call isp */
  if ( _ISR_Vector_table[ vector])
    (*_ISR_Vector_table[ vector ])( vector );

  _ISR_Local_disable( level );

  _Thread_Dispatch_unnest( _Per_CPU_Get() );

  _ISR_Nest_level--;

  if ( _ISR_Nest_level == 0 )
    /* restore old stack pointer */
    stack_ptr = _old_stack_ptr;

  _ISR_Local_enable( level );

  if ( _ISR_Nest_level )
    return;

  if ( !_Thread_Dispatch_is_enabled() ) {
    return;
  }

  if ( _Thread_Dispatch_necessary ) {
    _Thread_Dispatch();
  }
}
