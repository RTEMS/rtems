/*
 *  Install interrupt stack i960ka
 *
 *  $Id$
 */

#include <rtems.h>
#include <libcpu/i960KA.h>

extern i960_PRCB *Prcb;

void _CPU_Install_interrupt_stack( void )
{
  i960_PRCB *prcb = Prcb;
  unsigned32   level;
  
  /*  
   *  Set the Interrupt Stack in the PRCB and force a reload of it.
   *  Interrupts are disabled for safety.
   */

  _CPU_ISR_Disable( level );

    prcb->intr_stack = _CPU_Interrupt_stack_low;

    /* soft_reset( prcb ); XXX is there an action like this on the KA? */

  _CPU_ISR_Enable( level );
}

