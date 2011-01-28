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
 * Modified by T. Straumann for the beatnik BSP, 2005-2007
 * Some information may be based on mvme5500/irq/irq.h by K. Feng.
 */

#ifndef LIBBSP_POWERPC_MOT_PPC_NEW_IRQ_IRQ_H
#define LIBBSP_POWERPC_MOT_PPC_NEW_IRQ_IRQ_H

#define BSP_SHARED_HANDLER_SUPPORT      1
#include <rtems/irq.h>
#include <bsp/vectors.h>

/* This BSP also passes a pointer to the interrupt frame to the handler.
 * The PPC ABI guarantees that this will not mess up handlers written
 * without knowledge of this feature.
 */

typedef void (*BSP_rtems_irq_hdl)(rtems_irq_hdl_param,BSP_Exception_frame*);


/* legal priorities are 0 <= priority <= MAX_PRIO; 0 effectively disables the interrupt */
#define BSP_IRQ_MAX_PRIO		4
#define BSP_IRQ_MIN_PRIO		1

/* Note that priorites are only honoured for 'PCI' interrupt numbers.
 * The discovery pic has no support for hardware priorites; hence they
 * are handled in software
 */
#define BSP_IRQ_DEFAULT_PRIORITY 2


#define	BSP_PCI_IRQ_LOWEST_OFFSET	0	/* IMPLEMENTATION RELIES ON discovery pic INTERRUPTS HAVING NUMBERS 0..95 */
#define	BSP_IRQ_DEV			1	/* device interface interrupt */
#define	BSP_IRQ_DMA			2	/* DMA addres error interrupt (260) */
#define	BSP_IRQ_CPU			3	/* CPU interface interrupt */
#define	BSP_IRQ_IDMA0_1			4	/* IDMA ch. 0..1 complete interrupt (260) */
#define	BSP_IRQ_IDMA2_3			5	/* IDMA ch. 2..3 complete interrupt (260) */
#define	BSP_IRQ_IDMA4_5			6	/* IDMA ch. 4..5 complete interrupt (260) */
#define	BSP_IRQ_IDMA6_7			7	/* IDMA ch. 6..7 complete interrupt (260) */
#define	BSP_IRQ_TIME0_1			8	/* Timer 0..1 interrupt; Timer 0 on 64360 */
#define	BSP_IRQ_TIME2_3			9	/* Timer 2..3 interrupt; Timer 1 on 64360 */
#define	BSP_IRQ_TIME4_5			10	/* Timer 4..5 interrupt; Timer 2 on 64360 */
#define	BSP_IRQ_TIME6_7			11	/* Timer 6..7 interrupt; Timer 3 on 64360 */
#define	BSP_IRQ_PCI0_0			12	/* PCI 0 interrupt 0 summary (PCI 0 interrupt summary on 64360) */
#define	BSP_IRQ_PCI0_1			13	/* PCI 0 interrupt 1 summary (SRAM PAR ERROR on 64360)          */
#define	BSP_IRQ_PCI0_2			14	/* PCI 0 interrupt 2 summary */
#define	BSP_IRQ_PCI0_3			15	/* PCI 0 interrupt 3 summary */
#define	BSP_IRQ_PCI1_0			16	/* PCI 1 interrupt 0 summary (PCI 1 interrupt summary on 64360) */
#define	BSP_IRQ_ECC			    17	/* ECC error interrupt */
#define	BSP_IRQ_PCI1_1			18	/* PCI 1 interrupt 1 summary */
#define	BSP_IRQ_PCI1_2			19	/* PCI 1 interrupt 2 summary */
#define	BSP_IRQ_PCI1_3			20	/* PCI 1 interrupt 3 summary */
#define	BSP_IRQ_PCI0OUT_LO		21	/* PCI 0 outbound interrupt summary */
#define	BSP_IRQ_PCI0OUT_HI		22	/* PCI 0 outbound interrupt summary */
#define	BSP_IRQ_PCI1OUT_LO		23	/* PCI 1 outbound interrupt summary */
#define	BSP_IRQ_PCI1OUT_HI		24	/* PCI 1 outbound interrupt summary */
#define	BSP_IRQ_PCI0IN_LO		26	/* PCI 0 inbound interrupt summary */
#define	BSP_IRQ_PCI0IN_HI		27	/* PCI 0 inbound interrupt summary */
#define	BSP_IRQ_PCI1IN_LO		28	/* PCI 1 inbound interrupt summary */
#define	BSP_IRQ_PCI1IN_HI		29	/* PCI 1 inbound interrupt summary */
#define	BSP_IRQ_ETH0			(32+0)	/* Ethernet controller 0 interrupt */
#define	BSP_IRQ_ETH1			(32+1)	/* Ethernet controller 1 interrupt */
#define	BSP_IRQ_ETH2			(32+2)	/* Ethernet controller 2 interrupt */
#define	BSP_IRQ_SDMA			(32+4)	/* SDMA interrupt */
#define	BSP_IRQ_I2C			(32+5)	/* I2C interrupt */
#define	BSP_IRQ_BRG			(32+7)	/* Baud Rate Generator interrupt */
#define	BSP_IRQ_MPSC0			(32+8)	/* MPSC 0 interrupt */
#define	BSP_IRQ_MPSC1			(32+10)	/* MPSC 1 interrupt */
#define	BSP_IRQ_COMM			(32+11)	/* Comm unit interrupt */
#define	BSP_IRQ_GPP7_0			(32+24)	/* GPP[7..0] interrupt summary */
#define	BSP_IRQ_GPP15_8			(32+25)	/* GPP[15..8] interrupt summary */
#define	BSP_IRQ_GPP23_16   		(32+26)	/* GPP[23..16] interrupt summary */
#define	BSP_IRQ_GPP31_24  		(32+27)	/* GPP[31..24] interrupt summary */
#define	BSP_IRQ_GPP_0			64

#define	BSP_PCI_IRQ_NUMBER		(64+32)
#define	BSP_PCI_IRQ_MAX_OFFSET		(BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER - 1)

#define	BSP_PROCESSOR_IRQ_NUMBER 	1
#define	BSP_PROCESSOR_IRQ_LOWEST_OFFSET (BSP_PCI_IRQ_MAX_OFFSET+1)
#define	BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1)

/* summary */

#define	BSP_IRQ_NUMBER			(BSP_PCI_IRQ_NUMBER + BSP_PROCESSOR_IRQ_NUMBER)
#define	BSP_LOWEST_OFFSET		0
#define	BSP_MAX_OFFSET			(BSP_LOWEST_OFFSET + BSP_IRQ_NUMBER - 1)
#define	BSP_DECREMENTER			BSP_PROCESSOR_IRQ_LOWEST_OFFSET

#define BSP_UART_COM1_IRQ		BSP_IRQ_GPP_0
#define BSP_UART_COM2_IRQ		BSP_IRQ_GPP_0

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif


#include <bsp/irq_supp.h>

int  BSP_irq_is_enabled_at_pic(rtems_irq_number irq);

/* set priority of an interrupt; must not be called from ISR level */
int  BSP_irq_set_priority(rtems_irq_number irq, rtems_irq_prio pri);

/* Not for public use */
void BSP_rtems_irq_mng_init(unsigned cpuId);

#ifdef __cplusplus
}
#endif


#endif

#endif
