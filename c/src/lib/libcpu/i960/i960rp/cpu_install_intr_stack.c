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
  uint32_t     level;
  uint32_t   *isp = (int *) ISP_ADDR;
  
  /*  
   *  Set the Interrupt Stack in the on-CPU memory.
   *  Interrupts are disabled for safety.
   */

  _CPU_ISR_Disable( level );

    prcb->intr_stack = _CPU_Interrupt_stack_low;

    *isp = (uint32_t  ) prcb->intr_stack;

  _CPU_ISR_Enable( level );
}

