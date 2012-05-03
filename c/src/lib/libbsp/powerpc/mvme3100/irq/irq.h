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
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Adapted for the mvme3100 BSP by T. Straumann, 2007.
 */

#ifndef BSP_POWERPC_IRQ_H
#define BSP_POWERPC_IRQ_H

#define BSP_SHARED_HANDLER_SUPPORT      1
#include <rtems/irq.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 * rtems_irq_number Definitions
 */

/* Must pad number of external sources to 16 because
 * of the layout of vector/priority registers in the
 * 8540's openpic where there is a gap between
 * registers corresponding to external and core sources.
 */
#define BSP_EXT_IRQ_NUMBER          (16)
#define BSP_CORE_IRQ_NUMBER			(32)

/* openpic glue code from shared/irq assigns priorities and configures
 * initial ISRs for BSP_PCI_IRQ_NUMBER entries (plus ISA stuff on legacy
 * boards). Hence PCI_IRQ_NUMBER must also cover the internal sources
 * even though they have nothing to do with PCI.
 */
#define BSP_PCI_IRQ_NUMBER			(BSP_EXT_IRQ_NUMBER + BSP_CORE_IRQ_NUMBER)
#define BSP_PCI_IRQ_LOWEST_OFFSET	(0)
#define BSP_PCI_IRQ_MAX_OFFSET		(BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER - 1)

#define BSP_CORE_IRQ_LOWEST_OFFSET  (BSP_PCI_IRQ_LOWEST_OFFSET + BSP_EXT_IRQ_NUMBER)
#define BSP_CORE_IRQ_MAX_OFFSET		(BSP_CORE_IRQ_LOWEST_OFFSET + BSP_CORE_IRQ_NUMBER - 1)

/*
 * PowerPC exceptions handled as interrupt where an RTEMS managed interrupt
 * handler might be connected
 */
#define BSP_PROCESSOR_IRQ_NUMBER	(1)
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET (BSP_CORE_IRQ_MAX_OFFSET + 1)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1)
/* Misc vectors for OPENPIC irqs (IPI, timers)
 */
#define BSP_MISC_IRQ_NUMBER		(8)
#define BSP_MISC_IRQ_LOWEST_OFFSET	(BSP_PROCESSOR_IRQ_MAX_OFFSET + 1)
#define BSP_MISC_IRQ_MAX_OFFSET		(BSP_MISC_IRQ_LOWEST_OFFSET + BSP_MISC_IRQ_NUMBER - 1)
/*
 * Summary
 */
#define BSP_IRQ_NUMBER			(BSP_MISC_IRQ_MAX_OFFSET + 1)
#define BSP_LOWEST_OFFSET		(BSP_PCI_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET			(BSP_MISC_IRQ_MAX_OFFSET)

/*
 * Some PCI IRQ symbolic name definition
 */
#define BSP_PCI_IRQ0			(BSP_PCI_IRQ_LOWEST_OFFSET)

#define BSP_VME0_IRQ			(BSP_PCI_IRQ_LOWEST_OFFSET  +  0)
#define BSP_VME1_IRQ			(BSP_PCI_IRQ_LOWEST_OFFSET  +  1)
#define BSP_VME2_IRQ			(BSP_PCI_IRQ_LOWEST_OFFSET  +  2)
#define BSP_VME3_IRQ			(BSP_PCI_IRQ_LOWEST_OFFSET  +  3)

#define BSP_ABORT_IRQ			(BSP_PCI_IRQ_LOWEST_OFFSET  +  8)
#define BSP_TEMP_IRQ			(BSP_PCI_IRQ_LOWEST_OFFSET  +  9)
#define BSP_PHY_IRQ				(BSP_PCI_IRQ_LOWEST_OFFSET  + 10)
#define BSP_RTC_IRQ				(BSP_PCI_IRQ_LOWEST_OFFSET  + 11)

/* Weird - they provide 3 different IRQ lines per ethernet controller
 * but only one shared line for 2 UARTs ???
 */
#define BSP_UART_COM1_IRQ		(BSP_CORE_IRQ_LOWEST_OFFSET + 26)
#define BSP_UART_COM2_IRQ		(BSP_CORE_IRQ_LOWEST_OFFSET + 26)
#define BSP_I2C_IRQ				(BSP_CORE_IRQ_LOWEST_OFFSET + 27)

/*
 * Some internal (CORE) name definitions
 */
/* Ethernet (FEC) */
#define BSP_CORE_IRQ_FEC		(BSP_CORE_IRQ_LOWEST_OFFSET + 25)
/* i2c controller */
#define BSP_CORE_IRQ_I2C		(BSP_CORE_IRQ_LOWEST_OFFSET + 27)

/*
 * Some Processor execption handled as RTEMS IRQ symbolic name definition
 */
#define BSP_DECREMENTER			(BSP_PROCESSOR_IRQ_LOWEST_OFFSET)

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/

extern void BSP_rtems_irq_mng_init(unsigned cpuId);

#include <bsp/irq_supp.h>

#ifdef __cplusplus
};
#endif

#endif
#endif
