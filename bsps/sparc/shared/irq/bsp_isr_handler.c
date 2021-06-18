/*
*  COPYRIGHT (c) 2015
*  Cobham Gaisler
*
*  The license and distribution terms for this file may be
*  found in the file LICENSE in this distribution or at
*  http://www.rtems.org/license/LICENSE.
*
*/

#include <bsp.h>
#include <bsp/irq-generic.h>

/*
 * This function is called directly from _SPARC_Interrupt_trap() for
 * traps 0x10 to 0x1F which correspond to IRQ 0 to 15 respectively.
 */
void _SPARC_Interrupt_dispatch( uint32_t irq )
{
  bsp_interrupt_assert( irq < BSP_INTERRUPT_VECTOR_COUNT );

  /* Let BSP fixup and/or handle incoming IRQ */
  irq = bsp_irq_fixup( irq );

  bsp_interrupt_handler_dispatch_unchecked( irq );
}
