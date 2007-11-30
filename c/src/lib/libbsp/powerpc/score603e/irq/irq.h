/* irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by RTEMS to write interrupt handlers.
 *
 *  Copyright (C) 1999 valette@crf.canon.fr
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
 *  $Id$
 */

#ifndef LIBBSP_POWERPC_IRQ_H
#define LIBBSP_POWERPC_IRQ_H

#define BSP_SHARED_HANDLER_SUPPORT      1
#include <rtems/irq.h>

/*
 * 8259 edge/level control definitions at VIA
 */
#define ISA8259_M_ELCR 		0x4d0
#define ISA8259_S_ELCR 		0x4d1

#define ELCRS_INT15_LVL         0x80
#define ELCRS_INT14_LVL         0x40
#define ELCRS_INT13_LVL         0x20
#define ELCRS_INT12_LVL         0x10
#define ELCRS_INT11_LVL         0x08
#define ELCRS_INT10_LVL         0x04
#define ELCRS_INT9_LVL          0x02
#define ELCRS_INT8_LVL          0x01
#define ELCRM_INT7_LVL          0x80
#define ELCRM_INT6_LVL          0x40
#define ELCRM_INT5_LVL          0x20
#define ELCRM_INT4_LVL          0x10
#define ELCRM_INT3_LVL          0x8
#define ELCRM_INT2_LVL          0x4
#define ELCRM_INT1_LVL          0x2
#define ELCRM_INT0_LVL          0x1

    /* PIC's command and mask registers */
#define PIC_MASTER_COMMAND_IO_PORT		0x20	/* Master PIC command register */
#define PIC_SLAVE_COMMAND_IO_PORT		0xa0	/* Slave PIC command register */
#define PIC_MASTER_IMR_IO_PORT			0x21	/* Master PIC Interrupt Mask Register */
#define PIC_SLAVE_IMR_IO_PORT		 	0xa1	/* Slave PIC Interrupt Mask Register */

    /* Command for specific EOI (End Of Interrupt): Interrupt acknowledge */
#define PIC_EOSI	0x60	/* End of Specific Interrupt (EOSI) */
#define	SLAVE_PIC_EOSI  0x62	/* End of Specific Interrupt (EOSI) for cascade */
#define PIC_EOI		0x20	/* Generic End of Interrupt (EOI) */

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Symbolic IRQ names and related definitions
 */

  /*
   * ISA IRQ handler related definitions
   */
#define BSP_ISA_IRQ_NUMBER		(16)
#define BSP_ISA_IRQ_LOWEST_OFFSET	(0)
#define BSP_ISA_IRQ_MAX_OFFSET		(BSP_ISA_IRQ_LOWEST_OFFSET+BSP_ISA_IRQ_NUMBER-1)
  /*
   * PCI IRQ handlers related definitions
   * CAUTION : BSP_PCI_IRQ_LOWEST_OFFSET should be equal to OPENPIC_VEC_SOURCE
   */
#define BSP_PCI_IRQ_NUMBER		(16)
#define BSP_PCI_IRQ_LOWEST_OFFSET	(BSP_ISA_IRQ_NUMBER)
#define BSP_PCI_IRQ_MAX_OFFSET		(BSP_PCI_IRQ_LOWEST_OFFSET+BSP_PCI_IRQ_NUMBER-1)
  /*
   * PowerPC exceptions handled as interrupt where an RTEMS managed interrupt
   * handler might be connected
   */
#define BSP_PROCESSOR_IRQ_NUMBER	(1)
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET (BSP_PCI_IRQ_MAX_OFFSET + 1)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET+BSP_PROCESSOR_IRQ_NUMBER-1)
  /* Misc vectors for OPENPIC irqs (IPI, timers)
   */
#define BSP_MISC_IRQ_NUMBER		(8)
#define BSP_MISC_IRQ_LOWEST_OFFSET	(BSP_PROCESSOR_IRQ_MAX_OFFSET + 1)
#define BSP_MISC_IRQ_MAX_OFFSET		(BSP_MISC_IRQ_LOWEST_OFFSET+BSP_MISC_IRQ_NUMBER-1)
  /*
   * Summary
   */
#define BSP_IRQ_NUMBER			(BSP_MISC_IRQ_MAX_OFFSET + 1)
#define BSP_LOWEST_OFFSET		(BSP_ISA_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET		        (BSP_MISC_IRQ_MAX_OFFSET)
    /*
     * Some PCI IRQ symbolic name definition
     */
#define BSP_PCI_IRQ0			(BSP_PCI_IRQ_LOWEST_OFFSET)

    /*
     * Some Processor execption handled as RTEMS IRQ symbolic name definition
     */
#define BSP_DECREMENTER			(BSP_PROCESSOR_IRQ_LOWEST_OFFSET)

#ifdef __cplusplus
}
#endif

#endif

#endif
