/* irq_asm.h
 *
 *  This include file has defines to represent some contant used
 *  to program and manage the  Intel 8259 interrupt controller
 *
 *
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __IRQ_ASM_H__
#define __IRQ_ASM_H__

#define BSP_ASM_IRQ_VECTOR_BASE 0x20
    /* PIC's command and mask registers */
#define PIC_MASTER_COMMAND_IO_PORT		0x20	/* Master PIC command register */
#define PIC_SLAVE_COMMAND_IO_PORT		0xa0	/* Slave PIC command register */
#define PIC_MASTER_IMR_IO_PORT			0x21	/* Master PIC Interrupt Mask Register */
#define PIC_SLAVE_IMR_IO_PORT		 	0xa1	/* Slave PIC Interrupt Mask Register */

    /* Command for specific EOI (End Of Interrupt): Interrupt acknowledge */
#define PIC_EOSI	0x60	/* End of Specific Interrupt (EOSI) */
#define PIC_EOI		0x20	/* Generic End of Interrupt (EOI) */

#endif
