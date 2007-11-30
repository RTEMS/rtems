/* irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to write interrupt handlers.
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  This code is heavilly inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *      <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  irq.h,v 1.2.4.2 2003/09/04 18:45:20 joel Exp
 */

#ifndef LIBBSP_POWERPC_IRQ_H
#define LIBBSP_POWERPC_IRQ_H

#define BSP_SHARED_HANDLER_SUPPORT      1
#include <rtems/irq.h>

#define BSP_ASM_IRQ_VECTOR_BASE 0x0

#ifndef ASM

/*
 * Symblolic IRQ names and related definitions.
 */

#define BSP_PROCESSOR_IRQ_NUMBER	    (1)
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET	(0)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET+BSP_PROCESSOR_IRQ_NUMBER-1)

  /*
   * Summary
   */
#define BSP_IRQ_NUMBER		 	(BSP_PROCESSOR_IRQ_MAX_OFFSET + 1)
#define BSP_LOWEST_OFFSET		(BSP_PROCESSOR_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET			(BSP_IRQ_NUMBER - 1)

  /*
   * Some Processor execption handled as rtems IRQ symbolic name definition
   */
#define BSP_DECREMENTER			(BSP_PROCESSOR_IRQ_LOWEST_OFFSET)

/* dummy routines - there is no PIC */
void BSP_enable_irq_at_pic(const rtems_irq_number);
void BSP_disable_irq_at_pic(const rtems_irq_number);
int  BSP_setup_the_pic(rtems_irq_global_settings *);


#endif
#endif
