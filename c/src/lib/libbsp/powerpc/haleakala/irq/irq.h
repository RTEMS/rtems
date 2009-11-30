/*===============================================================*\
| Project: RTEMS Haleakala BSP                                    |
| by Michael Hamel ADInstruments Ltd 2008                         |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
\*===============================================================*/


#ifndef Haleakala_IRQ_IRQ_H
#define Haleakala_IRQ_IRQ_H

/* Implemented for us in bsp_irq_dispatch_list */
#define BSP_SHARED_HANDLER_SUPPORT      1

#include <rtems/irq.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

	/* Define UIC interrupt numbers; IRQs that cause an external interrupt that needs further decode.
	   These are arbitrary but it makes things easier if they match the CPU interrupt numbers */

	/*

		#define	BSP_UIC_UART0_GP		(BSP_UIC_IRQ_LOWEST_OFFSET + 0)
		#define	BSP_UIC_UART1			(BSP_UIC_IRQ_LOWEST_OFFSET + 1)
		#define	BSP_UIC_IIC0			(BSP_UIC_IRQ_LOWEST_OFFSET + 2)
		#define	BSP_UIC_ExtMaster		(BSP_UIC_IRQ_LOWEST_OFFSET + 3)
		#define	BSP_UIC_PCI				(BSP_UIC_IRQ_LOWEST_OFFSET + 4)
		#define	BSP_UIC_DMA0			(BSP_UIC_IRQ_LOWEST_OFFSET + 5)
		#define	BSP_UIC_DMA1			(BSP_UIC_IRQ_LOWEST_OFFSET + 6)
		#define	BSP_UIC_DMA2			(BSP_UIC_IRQ_LOWEST_OFFSET + 7)
		#define	BSP_UIC_DMA3			(BSP_UIC_IRQ_LOWEST_OFFSET + 8)
		#define	BSP_UIC_ENetWU			(BSP_UIC_IRQ_LOWEST_OFFSET + 9)
		#define	BSP_UIC_MALSERR			(BSP_UIC_IRQ_LOWEST_OFFSET + 10)
		#define	BSP_UIC_MALTXEOB		(BSP_UIC_IRQ_LOWEST_OFFSET + 11)
		#define	BSP_UIC_MALRXEOB		(BSP_UIC_IRQ_LOWEST_OFFSET + 12)
		#define	BSP_UIC_MALTXDE			(BSP_UIC_IRQ_LOWEST_OFFSET + 13)
		#define	BSP_UIC_MALRXDE			(BSP_UIC_IRQ_LOWEST_OFFSET + 14)
		#define	BSP_UIC_ENet			(BSP_UIC_IRQ_LOWEST_OFFSET + 15)
		#define	BSP_UIC_PCISERR			(BSP_UIC_IRQ_LOWEST_OFFSET + 16)
		#define	BSP_UIC_ECCERR			(BSP_UIC_IRQ_LOWEST_OFFSET + 17)
		#define	BSP_UIC_PCIPower		(BSP_UIC_IRQ_LOWEST_OFFSET + 18)
		#define	BSP_UIC_IRQ0			(BSP_UIC_IRQ_LOWEST_OFFSET + 25)
		#define	BSP_UIC_IRQ1			(BSP_UIC_IRQ_LOWEST_OFFSET + 26)
		#define	BSP_UIC_IRQ2			(BSP_UIC_IRQ_LOWEST_OFFSET + 27)
		#define	BSP_UIC_IRQ3			(BSP_UIC_IRQ_LOWEST_OFFSET + 28)
		#define	BSP_UIC_IRQ4			(BSP_UIC_IRQ_LOWEST_OFFSET + 29)
		#define	BSP_UIC_IRQ5			(BSP_UIC_IRQ_LOWEST_OFFSET + 30)
		#define	BSP_UIC_IRQ6			(BSP_UIC_IRQ_LOWEST_OFFSET + 31)

		#define	BSP_UIC_IRQ_NUMBER			(32)

	*/
	/* PPC405EX interrupt vectors */
	#define	BSP_UIC_UART1			(BSP_UIC_IRQ_LOWEST_OFFSET + 1)
	#define	BSP_UIC_IIC0			(BSP_UIC_IRQ_LOWEST_OFFSET + 2)
	#define	BSP_UIC_EIPPKP_READY	(BSP_UIC_IRQ_LOWEST_OFFSET + 3)
	#define	BSP_UIC_EIPPKP_TRNG		(BSP_UIC_IRQ_LOWEST_OFFSET + 4)
	#define	BSP_UIC_EBM				(BSP_UIC_IRQ_LOWEST_OFFSET + 5)
	#define	BSP_UIC_OPBtoPLB		(BSP_UIC_IRQ_LOWEST_OFFSET + 6)
	#define	BSP_UIC_IIC1			(BSP_UIC_IRQ_LOWEST_OFFSET + 7)
	#define	BSP_UIC_SPI				(BSP_UIC_IRQ_LOWEST_OFFSET + 8)
	#define	BSP_UIC_IRQ0			(BSP_UIC_IRQ_LOWEST_OFFSET + 9)
	#define	BSP_UIC_MALTXEOB		(BSP_UIC_IRQ_LOWEST_OFFSET + 10)
	#define	BSP_UIC_MALRXEOB		(BSP_UIC_IRQ_LOWEST_OFFSET + 11)
	#define	BSP_UIC_DMA0			(BSP_UIC_IRQ_LOWEST_OFFSET + 12)
	#define	BSP_UIC_DMA1			(BSP_UIC_IRQ_LOWEST_OFFSET + 13)
	#define	BSP_UIC_DMA2			(BSP_UIC_IRQ_LOWEST_OFFSET + 14)
	#define	BSP_UIC_DMA3			(BSP_UIC_IRQ_LOWEST_OFFSET + 15)
	#define	BSP_UIC_PCIe0AL			(BSP_UIC_IRQ_LOWEST_OFFSET + 16)
	#define	BSP_UIC_PCIe0VPD		(BSP_UIC_IRQ_LOWEST_OFFSET + 17)
	#define	BSP_UIC_PCIe0HRst		(BSP_UIC_IRQ_LOWEST_OFFSET + 18)
	#define	BSP_UIC_EIPPKP_PKA		(BSP_UIC_IRQ_LOWEST_OFFSET + 19)
	#define	BSP_UIC_PCIe0TCR		(BSP_UIC_IRQ_LOWEST_OFFSET + 20)
	#define	BSP_UIC_PCIe0VCO		(BSP_UIC_IRQ_LOWEST_OFFSET + 21)
	#define	BSP_UIC_EIPPKP_TRNG_AL	(BSP_UIC_IRQ_LOWEST_OFFSET + 22)
	#define	BSP_UIC_EIP94			(BSP_UIC_IRQ_LOWEST_OFFSET + 23)
	#define	BSP_UIC_EMAC0			(BSP_UIC_IRQ_LOWEST_OFFSET + 24)
	#define	BSP_UIC_EMAC1			(BSP_UIC_IRQ_LOWEST_OFFSET + 25)
	#define	BSP_UIC_UART0			(BSP_UIC_IRQ_LOWEST_OFFSET + 26)
	#define	BSP_UIC_IRQ4			(BSP_UIC_IRQ_LOWEST_OFFSET + 27)
	#define	BSP_UIC_UIC2_STD		(BSP_UIC_IRQ_LOWEST_OFFSET + 28)
	#define	BSP_UIC_UIC2_CRIT		(BSP_UIC_IRQ_LOWEST_OFFSET + 29)
	#define	BSP_UIC_UIC1_STD		(BSP_UIC_IRQ_LOWEST_OFFSET + 30)
	#define	BSP_UIC_UIC1_CRIT		(BSP_UIC_IRQ_LOWEST_OFFSET + 31)

	#define BSP_UIC1_IRQ_LOWEST_OFFSET	(BSP_UIC_IRQ_LOWEST_OFFSET + 32)
	#define	BSP_UIC_MALSERR			(BSP_UIC1_IRQ_LOWEST_OFFSET + 0)
	#define	BSP_UIC_MALTXDE			(BSP_UIC1_IRQ_LOWEST_OFFSET + 1)
	#define	BSP_UIC_MALRXDE			(BSP_UIC1_IRQ_LOWEST_OFFSET + 2)
	#define	BSP_UIC_PCIe0DCRErr		(BSP_UIC1_IRQ_LOWEST_OFFSET + 3)
	#define	BSP_UIC_PCIe1DCRErr		(BSP_UIC1_IRQ_LOWEST_OFFSET + 4)
	#define	BSP_UIC_ExtBus			(BSP_UIC1_IRQ_LOWEST_OFFSET + 5)
	#define	BSP_UIC_NDFC			(BSP_UIC1_IRQ_LOWEST_OFFSET + 6)
	#define	BSP_UIC_EIPKP_SLAVE		(BSP_UIC1_IRQ_LOWEST_OFFSET + 7)
	#define	BSP_UIC_GPT_TIMER5		(BSP_UIC1_IRQ_LOWEST_OFFSET + 8)
	#define	BSP_UIC_GPT_TIMER6		(BSP_UIC1_IRQ_LOWEST_OFFSET + 9)

	#define	BSP_UIC_GPT_TIMER0		(BSP_UIC1_IRQ_LOWEST_OFFSET + 16)
	#define	BSP_UIC_GPT_TIMER1		(BSP_UIC1_IRQ_LOWEST_OFFSET + 17)
	#define	BSP_UIC_IRQ7			(BSP_UIC1_IRQ_LOWEST_OFFSET + 18)
	#define	BSP_UIC_IRQ8			(BSP_UIC1_IRQ_LOWEST_OFFSET + 19)
	#define	BSP_UIC_IRQ9			(BSP_UIC1_IRQ_LOWEST_OFFSET + 20)
	#define	BSP_UIC_GPT_TIMER2		(BSP_UIC1_IRQ_LOWEST_OFFSET + 21)
	#define	BSP_UIC_GPT_TIMER3		(BSP_UIC1_IRQ_LOWEST_OFFSET + 22)
	#define	BSP_UIC_GPT_TIMER4		(BSP_UIC1_IRQ_LOWEST_OFFSET + 23)
	#define	BSP_UIC_SERIAL_ROM		(BSP_UIC1_IRQ_LOWEST_OFFSET + 24)
	#define	BSP_UIC_GPT_DEC			(BSP_UIC1_IRQ_LOWEST_OFFSET + 25)
	#define	BSP_UIC_IRQ2			(BSP_UIC1_IRQ_LOWEST_OFFSET + 26)
	#define	BSP_UIC_IRQ5			(BSP_UIC1_IRQ_LOWEST_OFFSET + 27)
	#define	BSP_UIC_IRQ6			(BSP_UIC1_IRQ_LOWEST_OFFSET + 28)
	#define	BSP_UIC_EMAC0WU			(BSP_UIC1_IRQ_LOWEST_OFFSET + 29)
	#define	BSP_UIC_IRQ1			(BSP_UIC1_IRQ_LOWEST_OFFSET + 30)
	#define	BSP_UIC_EMAC1WU			(BSP_UIC1_IRQ_LOWEST_OFFSET + 31)

	#define BSP_UIC2_IRQ_LOWEST_OFFSET	(BSP_UIC_IRQ_LOWEST_OFFSET + 64)
	#define	BSP_UIC_PCIe0INTA		(BSP_UIC2_IRQ_LOWEST_OFFSET + 0)
	#define	BSP_UIC_PCIe0INTB		(BSP_UIC2_IRQ_LOWEST_OFFSET + 1)
	#define	BSP_UIC_PCIe0INTC		(BSP_UIC2_IRQ_LOWEST_OFFSET + 2)
	#define	BSP_UIC_PCIe0INTD		(BSP_UIC2_IRQ_LOWEST_OFFSET + 3)
	#define	BSP_UIC_IRQ3			(BSP_UIC2_IRQ_LOWEST_OFFSET + 4)

	#define	BSP_UIC_USBOTG			(BSP_UIC2_IRQ_LOWEST_OFFSET + 30)

	#define	BSP_UIC_IRQ_NUMBER			(95)


	#define BSP_UIC_IRQ_LOWEST_OFFSET	0
	#define BSP_UIC_IRQ_MAX_OFFSET		(BSP_UIC_IRQ_LOWEST_OFFSET + BSP_UIC_IRQ_NUMBER - 1)

	#define BSP_UART_COM1_IRQ		BSP_UIC_UART0 /* Required by shared/console/uart.c */
	#define BSP_UART_COM2_IRQ		BSP_UIC_UART1

	/* Define processor IRQ numbers; IRQs that are handled by the exception vectors */

	#define BSP_PIT				BSP_PROCESSOR_IRQ_LOWEST_OFFSET	/* Required by ppc403/clock.c */
	#define	BSP_FIT				BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 1
	#define	BSP_WDOG			BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 2

	#define BSP_PROCESSOR_IRQ_NUMBER	    (3)
	#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET	(BSP_UIC_IRQ_MAX_OFFSET + 1)
	#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1)

	/* Summary and totals */

	#define BSP_IRQ_NUMBER		 	(BSP_PROCESSOR_IRQ_MAX_OFFSET + 1)
	#define BSP_LOWEST_OFFSET		(BSP_UIC_IRQ_LOWEST_OFFSET)
	#define BSP_MAX_OFFSET			(BSP_IRQ_NUMBER - 1)

	extern void BSP_rtems_irq_mng_init(unsigned cpuId);		// Implemented in irq_init.c
	#include <bsp/irq_supp.h>

	#ifdef __cplusplus
	}
	#endif
#endif /* ASM */

#endif /* Haleakala_IRQ_IRQ_H */
