/* irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *  CopyRight (C) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <irq.h>
#include <bsp.h>
#include <rtems/bspIo.h>

/*
 * default int vector
 */
extern void _ISR_Handler();

void default_int_handler()
{
  printk("raw_idt_notify has been called \n");
}

void  rtems_irq_mngt_init()
{
    int 			i;
    long			*vectorTable;
    rtems_interrupt_level       level;
    
    vectorTable = (long *) VECTOR_TABLE;
    
    _CPU_ISR_Disable(level);

    /* First, connect the ISR_Handler for IRQ and FIQ interrupts */
    _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, _ISR_Handler, NULL); 	
    _CPU_ISR_install_vector(ARM_EXCEPTION_FIQ, _ISR_Handler, NULL); 	   

     /* Initialize the vector table contents with default handler */
    for (i=0; i<BSP_MAX_INT; i++)
      *(vectorTable + i) = (long)(default_int_handler);

    /* Initialize the INT at the BSP level */
    BSP_rtems_irq_mngt_init();
}

