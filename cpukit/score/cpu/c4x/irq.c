/*
 *  C4x CPU Dependent Source
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
#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>

/*
 *  This routine provides the RTEMS interrupt management.
 */

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  unsigned long    *_old_stack_ptr;
#endif

register unsigned long  *stack_ptr asm("sp");

void __ISR_Handler(unsigned32 vector, void *isr_sp)
{
  register unsigned32 level;

  /* already disabled when we get here */
  /* _CPU_ISR_Disable( level ); */

  _Thread_Dispatch_disable_level++;

#if 0
  if ( stack_ptr > (_Thread_Executing->Start.stack +
            _Thread_Executing->Start.Initial_stack.size) ) {
    printk( "Blown interrupt stack at 0x%x\n", stack_ptr );
  }
#endif

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  if ( _ISR_Nest_level == 0 ) {
    /* Install irq stack */
    _old_stack_ptr = stack_ptr;
    stack_ptr = _CPU_Interrupt_stack_low;
  }
#endif

  _ISR_Nest_level++;

  /* leave it to the ISR to decide if they get reenabled */
  /* _CPU_ISR_Enable( level ); */

  /* call isp */
  if ( _ISR_Vector_table[ vector] )
    (*_ISR_Vector_table[ vector ])(
       vector, isr_sp - sizeof(CPU_Interrupt_frame) + 1 );

  _CPU_ISR_Disable( level );

  _ISR_Nest_level--;

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  if ( _ISR_Nest_level == 0 )       /* restore old stack pointer */
    stack_ptr = _old_stack_ptr;
#endif

  _Thread_Dispatch_disable_level--;

  _CPU_ISR_Enable( level );
  if ( _Thread_Dispatch_disable_level == 0 ) {
    if ( _Context_Switch_necessary || !_ISR_Signals_to_thread_executing ) {
       _ISR_Signals_to_thread_executing = FALSE;
       _Thread_Dispatch();
    }
  }
}
