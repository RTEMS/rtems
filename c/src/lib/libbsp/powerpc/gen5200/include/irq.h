/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005, 2010                     |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains declarations for the irq controller handler  |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       irq.h                                               */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 CPU interrupt header file             */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  This include file describe the data structure and   */
/*                 the functions implemented by rtems to write         */
/*                 interrupt handlers.                                 */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   MPC8260ads CPU interrupt header file                */
/*   Module:       irq.h                                               */
/*   Project:      RTEMS 4.6.0pre1 / MCF8260ads BSP                    */
/*   Version       1.1                                                 */
/*   Date:         10/10/2002                                          */
/*                                                                     */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*   Copyright (C) 1999 valette@crf.canon.fr                           */
/*                                                                     */
/*   This code is heavilly inspired by the public specification of     */
/*   STREAM V2 that can be found at:                                   */
/*                                                                     */
/*   <http://www.chorus.com/Documentation/index.html> by following     */
/*   the STREAM API Specification Document link.                       */
/*                                                                     */
/*   Modified for mpc8260 by Andy Dachs <a.dachs@sstl.co.uk>           */
/*   Surrey Satellite Technology Limited                               */
/*   The interrupt handling on the mpc8260 seems quite different from  */
/*   the 860 (I don't know the 860 well).  Although some interrupts    */
/*   are routed via the CPM irq and some are direct to the SIU they    */
/*   all appear logically the same.Therefore I removed the distinction */
/*   between SIU and CPM interrupts.                                   */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in the file LICENSE in this distribution or at     */
/*   http://www.rtems.com/license/LICENSE.                        */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Partially based on the code references which are named above.     */
/*   Adaptions, modifications, enhancements and any recent parts of    */
/*   the code are under the right of                                   */
/*                                                                     */
/*         IPR Engineering, Dachauer Straße 38, D-80335 München        */
/*                        Copyright(C) 2003                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   IPR Engineering makes no representation or warranties with        */
/*   respect to the performance of this computer program, and          */
/*   specifically disclaims any responsibility for any damages,        */
/*   special or consequential, connected with the use of this program. */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Version history:  1.0                                             */
/*                                                                     */
/***********************************************************************/

#ifndef LIBBSP_POWERPC_GEN5200_IRQ_H
#define LIBBSP_POWERPC_GEN5200_IRQ_H

#define PMCE_CE_SHADOW (1U << (31 - 31))
#define PMCE_CSE_STICKY (1U << (31 - 21))
#define PMCE_MSE_STICKY (1U << (31 - 10))
#define PMCE_PSE_STICKY (1U << (31 - 2))
#define PMCE_CSE_SOURCE(_pmce) (((_pmce) >> 8) & 0x3U)
#define PMCE_MSE_SOURCE(_pmce) (((_pmce) >> 16) & 0x1fU)
#define PMCE_PSE_SOURCE(_pmce) (((_pmce) >> 24) & 0x1fU)

/*
 * Peripheral IRQ handlers related definitions
 */
#define BSP_PER_IRQ_NUMBER		22
#define BSP_PER_IRQ_LOWEST_OFFSET	0
#define BSP_PER_IRQ_MAX_OFFSET		\
	(BSP_PER_IRQ_LOWEST_OFFSET + BSP_PER_IRQ_NUMBER - 1) /* 21 */
/*
 * Main IRQ handlers related definitions
 */
#define BSP_MAIN_IRQ_NUMBER		17
#define BSP_MAIN_IRQ_LOWEST_OFFSET	BSP_PER_IRQ_MAX_OFFSET + 1 /* 22 */
#define BSP_MAIN_IRQ_MAX_OFFSET		\
	(BSP_MAIN_IRQ_LOWEST_OFFSET + BSP_MAIN_IRQ_NUMBER - 1) /* 38 */
/*
 * Critical IRQ handlers related definitions
 */
#define BSP_CRIT_IRQ_NUMBER		4
#define BSP_CRIT_IRQ_LOWEST_OFFSET	BSP_MAIN_IRQ_MAX_OFFSET + 1 /* 39 */
#define BSP_CRIT_IRQ_MAX_OFFSET		\
	(BSP_CRIT_IRQ_LOWEST_OFFSET + BSP_CRIT_IRQ_NUMBER - 1) /* 42 */
/*
 * Summary of SIU interrupts
 */
#define BSP_SIU_IRQ_NUMBER		BSP_CRIT_IRQ_MAX_OFFSET + 1 /* 43 */
#define BSP_SIU_IRQ_LOWEST_OFFSET	BSP_PER_IRQ_LOWEST_OFFSET /* 0 */
#define BSP_SIU_IRQ_MAX_OFFSET		BSP_CRIT_IRQ_MAX_OFFSET	 /* 42 */
/*
 * Processor IRQ handlers related definitions
 */
#define BSP_PROCESSOR_IRQ_NUMBER	3
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET	BSP_CRIT_IRQ_MAX_OFFSET + 1 /* 44  */
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	\
        (BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1) /* 46 */
/*
 * Summary
 */
#define BSP_IRQ_NUMBER		BSP_PROCESSOR_IRQ_MAX_OFFSET + 1 /* 47 */
#define BSP_LOWEST_OFFSET	BSP_PER_IRQ_LOWEST_OFFSET /* 0 */
#define BSP_MAX_OFFSET		BSP_PROCESSOR_IRQ_MAX_OFFSET /* 46 */

#ifndef ASM

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

/*
 * index table for the module specific handlers, a few entries are only placeholders
 */
typedef enum {
  BSP_SIU_IRQ_SMARTCOMM		= BSP_PER_IRQ_LOWEST_OFFSET + 0,
  BSP_SIU_IRQ_PSC1		= BSP_PER_IRQ_LOWEST_OFFSET + 1,
  BSP_SIU_IRQ_PSC2		= BSP_PER_IRQ_LOWEST_OFFSET + 2,
  BSP_SIU_IRQ_PSC3		= BSP_PER_IRQ_LOWEST_OFFSET + 3,
  BSP_SIU_IRQ_PSC6		= BSP_PER_IRQ_LOWEST_OFFSET + 4,
  BSP_SIU_IRQ_ETH		= BSP_PER_IRQ_LOWEST_OFFSET + 5,
  BSP_SIU_IRQ_USB		= BSP_PER_IRQ_LOWEST_OFFSET + 6,
  BSP_SIU_IRQ_ATA		= BSP_PER_IRQ_LOWEST_OFFSET + 7,
  BSP_SIU_IRQ_PCI_CRT		= BSP_PER_IRQ_LOWEST_OFFSET + 8,
  BSP_SIU_IRQ_PCI_SC_RX	        = BSP_PER_IRQ_LOWEST_OFFSET + 9,
  BSP_SIU_IRQ_PCI_SC_TX	        = BSP_PER_IRQ_LOWEST_OFFSET + 10,
  BSP_SIU_IRQ_PSC4		= BSP_PER_IRQ_LOWEST_OFFSET + 11,
  BSP_SIU_IRQ_PSC5		= BSP_PER_IRQ_LOWEST_OFFSET + 12,
  BSP_SIU_IRQ_SPI_MODF	        = BSP_PER_IRQ_LOWEST_OFFSET + 13,
  BSP_SIU_IRQ_SPI_SPIF	        = BSP_PER_IRQ_LOWEST_OFFSET + 14,
  BSP_SIU_IRQ_I2C1		= BSP_PER_IRQ_LOWEST_OFFSET + 15,
  BSP_SIU_IRQ_I2C2		= BSP_PER_IRQ_LOWEST_OFFSET + 16,
  BSP_SIU_IRQ_MSCAN1		= BSP_PER_IRQ_LOWEST_OFFSET + 17,
  BSP_SIU_IRQ_MSCAN2		= BSP_PER_IRQ_LOWEST_OFFSET + 18,
  BSP_SIU_IRQ_IR_RX		= BSP_PER_IRQ_LOWEST_OFFSET + 19,
  BSP_SIU_IRQ_IR_TX		= BSP_PER_IRQ_LOWEST_OFFSET + 20,
  BSP_SIU_IRQ_XLB_ARB		= BSP_PER_IRQ_LOWEST_OFFSET + 21,

  /* SL_TIMER1 -- handler entry only used in case of SMI */
  BSP_SIU_IRQ_SL_TIMER1	        = BSP_MAIN_IRQ_LOWEST_OFFSET + 0,
  BSP_SIU_IRQ_IRQ1		= BSP_MAIN_IRQ_LOWEST_OFFSET + 1,
  BSP_SIU_IRQ_IRQ2		= BSP_MAIN_IRQ_LOWEST_OFFSET + 2,
  BSP_SIU_IRQ_IRQ3		= BSP_MAIN_IRQ_LOWEST_OFFSET + 3,
  /* LO_INT --  handler entry never used (only placeholder) */
  BSP_SIU_IRQ_LO_INT		= BSP_MAIN_IRQ_LOWEST_OFFSET + 4,
  BSP_SIU_IRQ_RTC_PER		= BSP_MAIN_IRQ_LOWEST_OFFSET + 5,
  BSP_SIU_IRQ_RTC_STW		= BSP_MAIN_IRQ_LOWEST_OFFSET + 6,
  BSP_SIU_IRQ_GPIO_STD		= BSP_MAIN_IRQ_LOWEST_OFFSET + 7,
  BSP_SIU_IRQ_GPIO_WKUP		= BSP_MAIN_IRQ_LOWEST_OFFSET + 8,
  BSP_SIU_IRQ_TMR0		= BSP_MAIN_IRQ_LOWEST_OFFSET + 9,
  BSP_SIU_IRQ_TMR1		= BSP_MAIN_IRQ_LOWEST_OFFSET + 10,
  BSP_SIU_IRQ_TMR2		= BSP_MAIN_IRQ_LOWEST_OFFSET + 1,
  BSP_SIU_IRQ_TMR3		= BSP_MAIN_IRQ_LOWEST_OFFSET + 12,
  BSP_SIU_IRQ_TMR4		= BSP_MAIN_IRQ_LOWEST_OFFSET + 13,
  BSP_SIU_IRQ_TMR5		= BSP_MAIN_IRQ_LOWEST_OFFSET + 14,
  BSP_SIU_IRQ_TMR6		= BSP_MAIN_IRQ_LOWEST_OFFSET + 15,
  BSP_SIU_IRQ_TMR7		= BSP_MAIN_IRQ_LOWEST_OFFSET + 16,

  BSP_SIU_IRQ_IRQ0		= BSP_CRIT_IRQ_LOWEST_OFFSET + 0,
  BSP_SIU_IRQ_SL_TIMER0		= BSP_CRIT_IRQ_LOWEST_OFFSET + 1,
  /* HI_INT -- handler entry never used (only placeholder) */
  BSP_SIU_IRQ_HI_INT		= BSP_CRIT_IRQ_LOWEST_OFFSET + 2,
  BSP_SIU_IRQ_CSS_WKUP		= BSP_CRIT_IRQ_LOWEST_OFFSET + 3,

  BSP_DECREMENTER		= BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 0,
  BSP_SYSMGMT			= BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 1,
  BSP_EXT                 	= BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 2
} rtems_irq_symbolic_name;

#define BSP_CRIT_IRQ_PRIO_LEVELS	  		  4
#define BSP_PERIODIC_TIMER                    BSP_SIU_IRQ_TMR6

#define BSP_INTERRUPT_VECTOR_MIN BSP_LOWEST_OFFSET

#define BSP_INTERRUPT_VECTOR_MAX BSP_MAX_OFFSET

#endif

#endif /* LIBBSP_POWERPC_GEN5200_IRQ_H */
