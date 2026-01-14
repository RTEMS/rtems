/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup i386_irq
 * @brief i8259 related 
 *
 * This include file has defines to represent some contant used
 * to program and manage the  Intel 8259 interrupt controller
 */

/*
 *
 * Copyright (c) 1998 Eric Valette <eric.valette@free.fr>
 *
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
