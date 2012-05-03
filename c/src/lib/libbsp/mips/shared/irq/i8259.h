/* irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by RTEMS to control the i8259 chip.
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
 */

#ifndef I8259_H
#define I8259_H

/*
 * 8259 edge/level control definitions at VIA
 */
#if 1 
#define ISA8259_M_ELCR 		0x4d0
#define ISA8259_S_ELCR 		0x4d1
#endif

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

/* 
 * PIC's command and mask registers 
 */
#define PIC_MASTER_COMMAND_IO_PORT		0x20	/* Master PIC command register */
#define PIC_SLAVE_COMMAND_IO_PORT		0xa0	/* Slave PIC command register */
#define PIC_MASTER_IMR_IO_PORT			0x21	/* Master PIC Interrupt Mask Register */
#define PIC_SLAVE_IMR_IO_PORT		 	0xa1	/* Slave PIC Interrupt Mask Register */

/* 
 * Command for specific EOI (End Of Interrupt): Interrupt acknowledge 
 */
#define PIC_EOSI	0x60	/* End of Specific Interrupt (EOSI) */
#define	SLAVE_PIC_EOSI  0x62	/* End of Specific Interrupt (EOSI) for cascade */
#define PIC_EOI		0x20	/* Generic End of Interrupt (EOI) */

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
 * rtems_irq_number Definitions
 */
#if 0
/*
 * ISA IRQ handler related definitions
 */
#define BSP_ISA_IRQ_NUMBER		(16)
#define BSP_ISA_IRQ_LOWEST_OFFSET	(0)
#define BSP_ISA_IRQ_MAX_OFFSET		(BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER - 1)

#ifndef qemu
#define BSP_PCI_IRQ_NUMBER		(16)
#else
#define BSP_PCI_IRQ_NUMBER		(0)
#endif
#define BSP_PCI_IRQ_LOWEST_OFFSET	(BSP_ISA_IRQ_NUMBER)
#define BSP_PCI_IRQ_MAX_OFFSET		(BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER - 1)

/*
 * PowerPC exceptions handled as interrupt where an RTEMS managed interrupt
 * handler might be connected
 */
#define BSP_PROCESSOR_IRQ_NUMBER	(1)
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET (BSP_PCI_IRQ_MAX_OFFSET + 1)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1)
/* Misc vectors for OPENPIC irqs (IPI, timers)
 */
#ifndef qemu
#define BSP_MISC_IRQ_NUMBER		(8)
#else
#define BSP_MISC_IRQ_NUMBER		(0)
#endif

#define BSP_MISC_IRQ_LOWEST_OFFSET	(BSP_PROCESSOR_IRQ_MAX_OFFSET + 1)
#define BSP_MISC_IRQ_MAX_OFFSET		(BSP_MISC_IRQ_LOWEST_OFFSET + BSP_MISC_IRQ_NUMBER - 1)
/*
 * Summary
 */
#define BSP_IRQ_NUMBER			(BSP_MISC_IRQ_MAX_OFFSET + 1)
#define BSP_LOWEST_OFFSET		(BSP_ISA_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET			(BSP_MISC_IRQ_MAX_OFFSET)
/*
 * Some ISA IRQ symbolic name definition
 */
#define BSP_ISA_PERIODIC_TIMER      	(0)
#define BSP_ISA_KEYBOARD          	(1)
#define BSP_ISA_UART_COM2_IRQ		(3)
#define BSP_ISA_UART_COM1_IRQ		(4)
#define BSP_ISA_RT_TIMER1	      	(8)
#define BSP_ISA_RT_TIMER3		(10)
/*
 * Some PCI IRQ symbolic name definition
 */
#define BSP_PCI_IRQ0			(BSP_PCI_IRQ_LOWEST_OFFSET)
#if     BSP_PCI_IRQ_NUMBER > 0
#define BSP_PCI_ISA_BRIDGE_IRQ		(BSP_PCI_IRQ0)
#endif

#if defined(mvme2100)
#define BSP_DEC21143_IRQ                (BSP_PCI_IRQ_LOWEST_OFFSET + 1)
#define BSP_PMC_PCMIP_TYPE1_SLOT0_IRQ   (BSP_PCI_IRQ_LOWEST_OFFSET + 2)
#define BSP_PCMIP_TYPE1_SLOT1_IRQ       (BSP_PCI_IRQ_LOWEST_OFFSET + 3)
#define BSP_PCMIP_TYPE2_SLOT0_IRQ       (BSP_PCI_IRQ_LOWEST_OFFSET + 4)
#define BSP_PCMIP_TYPE2_SLOT1_IRQ       (BSP_PCI_IRQ_LOWEST_OFFSET + 5)
#define BSP_PCI_INTA_UNIVERSE_LINT0_IRQ (BSP_PCI_IRQ_LOWEST_OFFSET + 7)
#define BSP_PCI_INTB_UNIVERSE_LINT1_IRQ (BSP_PCI_IRQ_LOWEST_OFFSET + 8)
#define BSP_PCI_INTC_UNIVERSE_LINT2_IRQ (BSP_PCI_IRQ_LOWEST_OFFSET + 9)
#define BSP_PCI_INTD_UNIVERSE_LINT3_IRQ (BSP_PCI_IRQ_LOWEST_OFFSET + 10)
#define BSP_UART_COM1_IRQ               (BSP_PCI_IRQ_LOWEST_OFFSET + 13)
#define BSP_FRONT_PANEL_ABORT_IRQ       (BSP_PCI_IRQ_LOWEST_OFFSET + 14)
#define BSP_RTC_IRQ                     (BSP_PCI_IRQ_LOWEST_OFFSET + 15)
#else
#define BSP_UART_COM1_IRQ		BSP_ISA_UART_COM1_IRQ
#define BSP_UART_COM2_IRQ		BSP_ISA_UART_COM2_IRQ
#endif

/*
 * Some Processor execption handled as RTEMS IRQ symbolic name definition
 */
#define BSP_DECREMENTER			(BSP_PROCESSOR_IRQ_LOWEST_OFFSET)
#endif

/*
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned short rtems_i8259_masks;
extern  volatile rtems_i8259_masks i8259s_cache;

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
/*
 * ------------------------ Intel 8259 (or emulation) Mngt Routines -------
 */
void BSP_i8259s_init(void);

/*
 * function to disable a particular irq at 8259 level. After calling
 * this function, even if the device asserts the interrupt line it will
 * not be propagated further to the processor
 *
 * RETURNS: 1/0 if the interrupt was enabled/disabled originally or
 *          a value < 0 on error.
 */
int BSP_irq_disable_at_i8259s        (const rtems_irq_number irqLine);
/*
 * function to enable a particular irq at 8259 level. After calling
 * this function, if the device asserts the interrupt line it will
 * be propagated further to the processor
 */
int BSP_irq_enable_at_i8259s		(const rtems_irq_number irqLine);
/*
 * function to acknowledge a particular irq at 8259 level. After calling
 * this function, if a device asserts an enabled interrupt line it will
 * be propagated further to the processor. Mainly usefull for people
 * writing raw handlers as this is automagically done for RTEMS managed
 * handlers.
 */
int BSP_irq_ack_at_i8259s           	(const rtems_irq_number irqLine);
/*
 * function to check if a particular irq is enabled at 8259 level. After calling
 */
int BSP_irq_enabled_at_i8259s        	(const rtems_irq_number irqLine);

int BSP_i8259s_int_process(void);

extern void BSP_rtems_irq_mng_init(unsigned cpuId);
extern void BSP_i8259s_init(void);

#ifdef __cplusplus
};
#endif

#endif
#endif
