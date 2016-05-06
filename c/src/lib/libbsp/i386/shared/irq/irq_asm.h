/**
 * @file
 * @ingroup i386_irq
 * @brief
 */

/* irq_asm.h
 *
 *  This include file has defines to represent some contant used
 *  to program and manage the  Intel 8259 interrupt controller
 *
 *
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *
 *  Copyright (c) 2016 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
#ifndef __I8259S_H__
#define __I8259S_H__

#define BSP_ASM_IRQ_VECTOR_BASE 0x20
    /** @brief PIC's command and mask registers */
#define PIC_MASTER_COMMAND_IO_PORT		0x20	///< Master PIC command register
#define PIC_SLAVE_COMMAND_IO_PORT		0xa0	///< Slave PIC command register
#define PIC_MASTER_IMR_IO_PORT			0x21	///< Master PIC Interrupt Mask Register
#define PIC_SLAVE_IMR_IO_PORT		 	0xa1	///< Slave PIC Interrupt Mask Register

    /** @brief Command for specific EOI (End Of Interrupt): Interrupt acknowledge */
#define PIC_EOSI	0x60	///< End of Specific Interrupt (EOSI)
#define PIC_EOI		0x20	///< Generic End of Interrupt (EOI)

/* Operation control word type 3.  Bit 3 (0x08) must be set. Even address. */
#define PIC_OCW3_RIS        0x01            /* 1 = read IS, 0 = read IR */
#define PIC_OCW3_RR         0x02            /* register read */
#define PIC_OCW3_P          0x04            /* poll mode command */
/* 0x08 must be 1 to select OCW3 vs OCW2 */
#define PIC_OCW3_SEL        0x08            /* must be 1 */
/* 0x10 must be 0 to select OCW3 vs ICW1 */
#define PIC_OCW3_SMM        0x20            /* special mode mask */
#define PIC_OCW3_ESMM       0x40            /* enable SMM */

#endif
