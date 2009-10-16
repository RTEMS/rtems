/* 
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

#ifndef BSP_POWERPC_IRQ_H
#define BSP_POWERPC_IRQ_H

#define BSP_SHARED_HANDLER_SUPPORT      1
#include <rtems/irq.h>

/* PIC's command and mask registers */
#define PIC_MASTER_COMMAND_IO_PORT 0x20 /* Master PIC command register */
#define PIC_SLAVE_COMMAND_IO_PORT  0xa0 /* Slave PIC command register */
#define PIC_MASTER_IMR_IO_PORT     0x21 /* Master PIC Interrupt Mask Register */
#define PIC_SLAVE_IMR_IO_PORT      0xa1 /* Slave PIC Interrupt Mask Register */

/* Command for specific EOI (End Of Interrupt): Interrupt acknowledge */
#define PIC_EOSI        0x60  /* End of Specific Interrupt (EOSI) */
#define SLAVE_PIC_EOSI  0x62 /* End of Specific Interrupt (EOSI) for cascade */
#define PIC_EOI         0x20  /* Generic End of Interrupt (EOI) */

#ifndef ASM
#ifdef __cplusplus
extern "C" {
#endif

/*
 * rtems_irq_number Definitions
 */

/*
 * ISA IRQ handler related definitions
 */
#define BSP_ISA_IRQ_NUMBER         (16)
#define BSP_ISA_IRQ_LOWEST_OFFSET  (0)
#define BSP_ISA_IRQ_MAX_OFFSET \
        (BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER - 1)
/*
 * PCI IRQ handlers related definitions
 * CAUTION : BSP_PCI_IRQ_LOWEST_OFFSET should be equal to OPENPIC_VEC_SOURCE
 */
#define BSP_PCI_IRQ_NUMBER        (26)
#define BSP_PCI_IRQ_LOWEST_OFFSET (BSP_ISA_IRQ_NUMBER)
#define BSP_PCI_IRQ_MAX_OFFSET    \
        (BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER - 1)

/*
 * PowerPC exceptions handled as interrupt where an RTEMS managed interrupt
 * handler might be connected
 */
#define BSP_PROCESSOR_IRQ_NUMBER        (1)
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET (BSP_PCI_IRQ_MAX_OFFSET + 1)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET \
        (BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1)
/*
 * Misc vectors for OPENPIC irqs (IPI, timers)
 */
#define BSP_MISC_IRQ_NUMBER        (8)
#define BSP_MISC_IRQ_LOWEST_OFFSET (BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 1)
#define BSP_MISC_IRQ_MAX_OFFSET \
        (BSP_MISC_IRQ_LOWEST_OFFSET + BSP_MISC_IRQ_NUMBER - 1)
/*
 * Summary
 */
#define BSP_IRQ_NUMBER      (BSP_MISC_IRQ_MAX_OFFSET + 1)
#define BSP_LOWEST_OFFSET   (BSP_ISA_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET      (BSP_MISC_IRQ_MAX_OFFSET)


/*
 * Some PCI IRQ symbolic name definition
 */
#define BSP_PCI_IRQ0      (BSP_PCI_IRQ_LOWEST_OFFSET)

/*
 * Some Processor execption handled as RTEMS IRQ symbolic name definition
 */
#define BSP_DECREMENTER   (BSP_PROCESSOR_IRQ_LOWEST_OFFSET)

/*
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned short rtems_i8259_masks;
extern  volatile rtems_i8259_masks i8259s_cache;

/* Stuff in irq_supp.h should eventually go into <rtems/irq.h> */
#include <bsp/irq_supp.h>

#ifdef __cplusplus
};
#endif

#endif
#endif
