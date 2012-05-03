/* irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by RTEMS to write interrupt handlers.
 *
 *  This code is heavilly inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *  <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

/*
 * ISA IRQ handler related definitions
 */
#define BSP_ISA_IRQ_NUMBER		(16)
#define BSP_ISA_IRQ_LOWEST_OFFSET	(0)
#define BSP_ISA_IRQ_MAX_OFFSET		(BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER - 1)

/*
 * PCI IRQ handlers related definitions
 */
#define BSP_PCI_IRQ_NUMBER		(16)
#define BSP_PCI_IRQ_LOWEST_OFFSET	(BSP_ISA_IRQ_NUMBER)
#define BSP_PCI_IRQ_MAX_OFFSET		(BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER - 1)

/*
 * PMC IRQ
 */
#define BSP_PMC_IRQ_NUMBER              (4)
#define BSP_PMC_IRQ_LOWEST_OFFSET	(BSP_PCI_IRQ_MAX_OFFSET + 1)
#define BSP_PMC_IRQ_MAX_OFFSET		(BSP_PMC_IRQ_LOWEST_OFFSET + BSP_PMC_IRQ_NUMBER - 1)


/*
 * PowerPC exceptions handled as interrupt where an RTEMS managed interrupt
 * handler might be connected
 */
#define BSP_PROCESSOR_IRQ_NUMBER	(1)
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET (BSP_PMC_IRQ_MAX_OFFSET + 1)
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
#define BSP_LOWEST_OFFSET		(BSP_ISA_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET			(BSP_MISC_IRQ_MAX_OFFSET)

/*
 * Some Processor execption handled as RTEMS IRQ symbolic name definition
 */
#define BSP_DECREMENTER			(BSP_PROCESSOR_IRQ_LOWEST_OFFSET)

/*
 * First Score Unique IRQ
 */
#define Score_IRQ_First ( BSP_PCI_IRQ_LOWEST_OFFSET )

/*
 * The Following Are part of a Score603e FPGA.
 */
#define SCORE603E_IRQ00   ( Score_IRQ_First +  0 )
#define SCORE603E_IRQ01   ( Score_IRQ_First +  1 )
#define SCORE603E_IRQ02   ( Score_IRQ_First +  2 )
#define SCORE603E_IRQ03   ( Score_IRQ_First +  3 )
#define SCORE603E_IRQ04   ( Score_IRQ_First +  4 )
#define SCORE603E_IRQ05   ( Score_IRQ_First +  5 )
#define SCORE603E_IRQ06   ( Score_IRQ_First +  6 )
#define SCORE603E_IRQ07   ( Score_IRQ_First +  7 )
#define SCORE603E_IRQ08   ( Score_IRQ_First +  8 )
#define SCORE603E_IRQ09   ( Score_IRQ_First +  9 )
#define SCORE603E_IRQ10   ( Score_IRQ_First + 10 )
#define SCORE603E_IRQ11   ( Score_IRQ_First + 11 )
#define SCORE603E_IRQ12   ( Score_IRQ_First + 12 )
#define SCORE603E_IRQ13   ( Score_IRQ_First + 13 )
#define SCORE603E_IRQ14   ( Score_IRQ_First + 14 )
#define SCORE603E_IRQ15   ( Score_IRQ_First + 15 )

#define SCORE603E_TIMER1_IRQ           SCORE603E_IRQ00
#define SCORE603E_TIMER2_IRQ           SCORE603E_IRQ01
#define SCORE603E_TIMER3_IRQ           SCORE603E_IRQ02
#define SCORE603E_85C30_1_IRQ          SCORE603E_IRQ03
#define SCORE603E_85C30_0_IRQ          SCORE603E_IRQ04
#define SCORE603E_RTC_IRQ              SCORE603E_IRQ05
#define SCORE603E_PCI_IRQ_0            SCORE603E_IRQ06
#define SCORE603E_PCI_IRQ_1            SCORE603E_IRQ07
#define SCORE603E_PCI_IRQ_2            SCORE603E_IRQ08
#define SCORE603E_PCI_IRQ_3            SCORE603E_IRQ09
#define SCORE603E_UNIVERSE_IRQ         SCORE603E_IRQ10
#define SCORE603E_1553_IRQ             SCORE603E_IRQ11
#define SCORE603E_MAIL_BOX_IRQ_0       SCORE603E_IRQ12
#define SCORE603E_MAIL_BOX_IRQ_1       SCORE603E_IRQ13
#define SCORE603E_MAIL_BOX_IRQ_2       SCORE603E_IRQ14
#define SCORE603E_MAIL_BOX_IRQ_3       SCORE603E_IRQ15

/*
 * The Score FPGA maps all interrupts comming from the PMC card to
 * the FPGA interrupt SCORE603E_PCI_IRQ_0 the PMC status word must be
 * read to indicate which interrupt was chained to the FPGA.
 */
#define SCORE603E_IRQ16   ( Score_IRQ_First + 16 )
#define SCORE603E_IRQ17   ( Score_IRQ_First + 17 )
#define SCORE603E_IRQ18   ( Score_IRQ_First + 18 )
#define SCORE603E_IRQ19   ( Score_IRQ_First + 19 )

/*
 * IRQ'a read from the PMC card
 */
#define SCORE603E_85C30_4_IRQ          SCORE603E_IRQ16    /* SCC 422-1 */
#define SCORE603E_85C30_2_IRQ          SCORE603E_IRQ17    /* SCC 232-1 */
#define SCORE603E_85C30_5_IRQ          SCORE603E_IRQ18    /* SCC 422-2 */
#define SCORE603E_85C30_3_IRQ          SCORE603E_IRQ19    /* SCC 232-2 */

#define MAX_BOARD_IRQS                 SCORE603E_IRQ19

extern void BSP_rtems_irq_mng_init(unsigned cpuId);

#ifdef __cplusplus
}
#endif

#endif
#endif
