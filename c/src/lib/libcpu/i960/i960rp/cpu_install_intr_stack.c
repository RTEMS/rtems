/*
 *  Install interrupt stack i960RP
 *
 *  $Id$
 */

#include <rtems.h>
#include <libcpu/i960RP.h>

extern i960_PRCB *Prcb;

void _CPU_Install_interrupt_stack( void )
{
  i960_PRCB *prcb = Prcb;
  unsigned32   level;
  unsigned32 *isp = (int *) ISP_ADDR;
  
  /*  
   *  Set the Interrupt Stack in the on-CPU memory.
   *  Interrupts are disabled for safety.
   */

  _CPU_ISR_Disable( level );

    prcb->intr_stack = _CPU_Interrupt_stack_low;

    *isp = (unsigned32) prcb->intr_stack;

  _CPU_ISR_Enable( level );
}

