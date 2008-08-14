/*  Blackfin CPU Dependent Source
 *
 *  COPYRIGHT (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 

#if 0 /* this file no longer used */
 
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

register unsigned long  *stack_ptr asm("SP");

void ISR_Handler2(uint32_t   vector, void *isr_sp)
{
  register uint32_t   level;

  _CPU_ISR_Disable( level );

  _Thread_Dispatch_disable_level++;

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  if ( _ISR_Nest_level == 0 ) {
    /* Install irq stack */
    _old_stack_ptr = stack_ptr;
    stack_ptr = _CPU_Interrupt_stack_high;
  }
#endif

  _ISR_Nest_level++;

  /* leave it to the ISR to decide if they get reenabled */
  _CPU_ISR_Enable( level );

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

  if ( _ISR_Nest_level )
    return;

  if ( _Thread_Dispatch_disable_level ) {
    _ISR_Signals_to_thread_executing = FALSE;
    return;
  }

  if ( _Context_Switch_necessary || _ISR_Signals_to_thread_executing ) {
    _ISR_Signals_to_thread_executing = FALSE;
    _ISR_Thread_Dispatch();
    /*_Thread_Running->Registers.register_rets = current_thread_pc;*/
  }
}

uint32_t SIC_IAR_Value ( uint8_t Vector )
{
  switch ( Vector ){
    case 7:
      return 0x00000000;
    case 8:
      return 0x11111111;
    case 9:
      return 0x22222222;
    case 10:
      return 0x33333333;
    case 11:
      return 0x44444444;
    case 12:
      return 0x55555555;
    case 13:
      return 0x66666666;
    case 14:
      return 0x77777777;
    case 15:
      return 0x88888888;
  }
}

#endif /* 0 */

