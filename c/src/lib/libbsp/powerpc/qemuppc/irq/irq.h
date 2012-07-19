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
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_IRQ_H
#define LIBBSP_POWERPC_IRQ_H

#define BSP_SHARED_HANDLER_SUPPORT      1

#ifndef ASM

#include <rtems/irq.h>


/*
 * Symblolic IRQ names and related definitions.
 */

/*
 * PCI IRQ handlers related definitions
 * CAUTION : BSP_PCI_IRQ_LOWEST_OFFSET should be equal to OPENPIC_VEC_SOURCE
 */
/* FIXME: do we need PCI interrrupts here ? */
#define BSP_PCI_IRQ_NUMBER		(16)
#define BSP_PCI_IRQ_LOWEST_OFFSET	(0)
#define BSP_PCI_IRQ_MAX_OFFSET		(BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER - 1)

#define BSP_PROCESSOR_IRQ_NUMBER	    (1)
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET	(BSP_PCI_IRQ_MAX_OFFSET)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET+BSP_PROCESSOR_IRQ_NUMBER-1)


  /*
   * Summary
   */
#define BSP_IRQ_NUMBER		 	(BSP_PROCESSOR_IRQ_MAX_OFFSET + 1)
#define BSP_LOWEST_OFFSET		(BSP_PCI_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET			(BSP_IRQ_NUMBER - 1)

  /*
   * Some Processor execption handled as rtems IRQ symbolic name definition
   */
#define BSP_DECREMENTER			(BSP_PROCESSOR_IRQ_LOWEST_OFFSET)

#define BSP_INTERRUPT_VECTOR_MIN BSP_LOWEST_OFFSET

#define BSP_INTERRUPT_VECTOR_MAX BSP_MAX_OFFSET

/* #include <bsp/irq_supp.h> */

#ifdef __cplusplus
extern "C" {
#endif

void BSP_rtems_irq_mng_init(unsigned cpuId);

#ifdef __cplusplus
}
#endif

#endif
#endif
