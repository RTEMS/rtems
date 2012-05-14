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
 *
 * Copyright 2004, 2005 Brookhaven National Laboratory and
 *                 Shuchen Kate Feng <feng1@bnl.gov>
 *
 *    - modified shared/irq/irq.h for Mvme5500 (no ISA devices/PIC)
 *    - Discovery GT64260 interrupt controller instead of 8259.
 *    - Added support for software IRQ priority levels.
 *    - modified to optimize the IRQ latency and handling
 */

#ifndef LIBBSP_POWERPC_MVME5500_IRQ_IRQ_H
#define LIBBSP_POWERPC_MVME5500_IRQ_IRQ_H

#define BSP_SHARED_HANDLER_SUPPORT      1
#include <rtems/irq.h>

#ifndef ASM

#define OneTierIrqPrioTbl 1

/*
 * Symbolic IRQ names and related definitions.
 */

/* leave the ISA symbols in there, so we can reuse shared/irq.c
 * Also, we start numbering PCI irqs at 16 because the OPENPIC
 * driver relies on this when mapping irq number <-> vectors
 * (OPENPIC_VEC_SOURCE in openpic.h)
 */

  /* See section 25.2 , Table 734 of GT64260 controller
   * Main Interrupt Cause Low register
   */
#define BSP_MICL_IRQ_NUMBER           (32)
#define BSP_MICL_IRQ_LOWEST_OFFSET    (0)
#define BSP_MICL_IRQ_MAX_OFFSET       (BSP_MICL_IRQ_LOWEST_OFFSET + BSP_MICL_IRQ_NUMBER -1)
  /*
   * Main Interrupt Cause High register
   */
#define BSP_MICH_IRQ_NUMBER           (32)
#define BSP_MICH_IRQ_LOWEST_OFFSET    (BSP_MICL_IRQ_MAX_OFFSET+1)
#define BSP_MICH_IRQ_MAX_OFFSET       (BSP_MICH_IRQ_LOWEST_OFFSET + BSP_MICH_IRQ_NUMBER -1)
 /* External GPP Interrupt assignements
  */
#define BSP_GPP_IRQ_NUMBER		(32)
#define BSP_GPP_IRQ_LOWEST_OFFSET	(BSP_MICH_IRQ_MAX_OFFSET+1)
#define BSP_GPP_IRQ_MAX_OFFSET	  	(BSP_GPP_IRQ_LOWEST_OFFSET + BSP_GPP_IRQ_NUMBER - 1)

 /*
  * PowerPc exceptions handled as interrupt where a rtems managed interrupt
  * handler might be connected
  */
#define BSP_PROCESSOR_IRQ_NUMBER	 (1)
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET (BSP_GPP_IRQ_MAX_OFFSET + 1)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1)

  /* allow a couple of vectors for VME and counter/timer irq sources etc.
   * This is probably not needed any more.
   */
#define BSP_MISC_IRQ_NUMBER		(30)
#define BSP_MISC_IRQ_LOWEST_OFFSET	(BSP_PROCESSOR_IRQ_MAX_OFFSET + 1)
#define BSP_MISC_IRQ_MAX_OFFSET	(BSP_MISC_IRQ_LOWEST_OFFSET + BSP_MISC_IRQ_NUMBER - 1)

  /*
   * Summary
   */
#define BSP_IRQ_NUMBER			(BSP_MISC_IRQ_MAX_OFFSET + 1)
#define BSP_MAIN_IRQ_NUMBER             (64)
#define BSP_PIC_IRQ_NUMBER              (96)
#define BSP_LOWEST_OFFSET		(BSP_MICL_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET			(BSP_MISC_IRQ_MAX_OFFSET)

  /* Main CPU interrupt cause (Low) */
#define BSP_MAIN_TIMER0_1_IRQ         (BSP_MICL_IRQ_LOWEST_OFFSET+8)
#define BSP_MAIN_PCI0_7_0             (BSP_MICL_IRQ_LOWEST_OFFSET+12)
#define BSP_MAIN_PCI0_15_8            (BSP_MICL_IRQ_LOWEST_OFFSET+13)
#define BSP_MAIN_PCI0_23_16           (BSP_MICL_IRQ_LOWEST_OFFSET+14)
#define BSP_MAIN_PCI0_31_24           (BSP_MICL_IRQ_LOWEST_OFFSET+15)
#define BSP_MAIN_PCI1_7_0             (BSP_MICL_IRQ_LOWEST_OFFSET+16)
#define BSP_MAIN_PCI1_15_8            (BSP_MICL_IRQ_LOWEST_OFFSET+18)
#define BSP_MAIN_PCI1_23_16           (BSP_MICL_IRQ_LOWEST_OFFSET+19)
#define BSP_MAIN_PCI1_31_24           (BSP_MICL_IRQ_LOWEST_OFFSET+20)


  /* Main CPU interrupt cause (High) */
#define BSP_MAIN_ETH0_IRQ             (BSP_MICH_IRQ_LOWEST_OFFSET)
#define BSP_MAIN_ETH1_IRQ             (BSP_MICH_IRQ_LOWEST_OFFSET+1)
#define BSP_MAIN_ETH2_IRQ             (BSP_MICH_IRQ_LOWEST_OFFSET+2)
#define BSP_MAIN_GPP7_0_IRQ           (BSP_MICH_IRQ_LOWEST_OFFSET+24)
#define BSP_MAIN_GPP15_8_IRQ          (BSP_MICH_IRQ_LOWEST_OFFSET+25)
#define BSP_MAIN_GPP23_16_IRQ         (BSP_MICH_IRQ_LOWEST_OFFSET+26)
#define BSP_MAIN_GPP31_24_IRQ         (BSP_MICH_IRQ_LOWEST_OFFSET+27)

  /* on the MVME5500, these are the GT64260B external GPP0 interrupt */
#define BSP_PCI_IRQ_LOWEST_OFFSET       (BSP_GPP_IRQ_LOWEST_OFFSET)
#define BSP_UART_COM2_IRQ		(BSP_GPP_IRQ_LOWEST_OFFSET)
#define BSP_UART_COM1_IRQ		(BSP_GPP_IRQ_LOWEST_OFFSET)
#define BSP_GPP8_IRQ_OFFSET		(BSP_GPP_IRQ_LOWEST_OFFSET+8)
#define BSP_GPP_PMC1_INTA 		(BSP_GPP8_IRQ_OFFSET)
#define BSP_GPP16_IRQ_OFFSET          (BSP_GPP_IRQ_LOWEST_OFFSET+16)
#define BSP_GPP24_IRQ_OFFSET          (BSP_GPP_IRQ_LOWEST_OFFSET+24)
#define BSP_GPP_VME_VLINT0            (BSP_GPP_IRQ_LOWEST_OFFSET+12)
#define BSP_GPP_VME_VLINT1            (BSP_GPP_IRQ_LOWEST_OFFSET+13)
#define BSP_GPP_VME_VLINT2            (BSP_GPP_IRQ_LOWEST_OFFSET+14)
#define BSP_GPP_VME_VLINT3            (BSP_GPP_IRQ_LOWEST_OFFSET+15)
#define BSP_GPP_PMC2_INTA             (BSP_GPP_IRQ_LOWEST_OFFSET+16)
#define BSP_GPP_82544_IRQ             (BSP_GPP_IRQ_LOWEST_OFFSET+20)
#define BSP_GPP_WDT_NMI_IRQ           (BSP_GPP_IRQ_LOWEST_OFFSET+24)
#define BSP_GPP_WDT_EXP_IRQ           (BSP_GPP_IRQ_LOWEST_OFFSET+25)

 /*
   * Some Processor execption handled as rtems IRQ symbolic name definition
   */
#define BSP_DECREMENTER		(BSP_PROCESSOR_IRQ_LOWEST_OFFSET)

extern void BSP_rtems_irq_mng_init(unsigned cpuId);

#include <bsp/irq_supp.h>

#endif
#endif
