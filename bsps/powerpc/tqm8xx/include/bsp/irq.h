/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS TQM8xx BSP
 *
 * This file contains the console driver.
 *
 * Derived from: generic MPC83xx BSP
 */

/*
 * Copyright (c) 2008 Thomas Doerfler, embedded brains GmbH & Co. KG
 * All rights reserved.
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

#ifndef TQM8xx_IRQ_IRQ_H
#define TQM8xx_IRQ_IRQ_H

#include <stdbool.h>

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

/*
 * the following definitions specify the indices used
 * to interface the interrupt handler API
 */

/*
 * Peripheral IRQ handlers related definitions
 */
#define BSP_SIU_PER_IRQ_NUMBER	 16
#define BSP_SIU_IRQ_LOWEST_OFFSET 0
#define BSP_SIU_IRQ_MAX_OFFSET   (BSP_SIU_IRQ_LOWEST_OFFSET\
                                  +BSP_SIU_PER_IRQ_NUMBER-1)

#define BSP_IS_SIU_IRQ(irqnum)				\
          (((irqnum) >= BSP_SIU_IRQ_LOWEST_OFFSET) &&	\
	   ((irqnum) <= BSP_SIU_IRQ_MAX_OFFSET))

#define BSP_CPM_PER_IRQ_NUMBER	 32
#define BSP_CPM_IRQ_LOWEST_OFFSET (BSP_SIU_IRQ_MAX_OFFSET+1)
#define BSP_CPM_IRQ_MAX_OFFSET   (BSP_CPM_IRQ_LOWEST_OFFSET\
                                  +BSP_CPM_PER_IRQ_NUMBER-1)

#define BSP_IS_CPM_IRQ(irqnum)				\
          (((irqnum) >= BSP_CPM_IRQ_LOWEST_OFFSET) &&	\
	   ((irqnum) <= BSP_CPM_IRQ_MAX_OFFSET))
/*
 * Processor IRQ handlers related definitions
 */
#define BSP_PROCESSOR_IRQ_NUMBER        1
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET	(BSP_CPM_IRQ_MAX_OFFSET+1)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET\
                                         +BSP_PROCESSOR_IRQ_NUMBER-1)

#define BSP_IS_PROCESSOR_IRQ(irqnum)				\
          (((irqnum) >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET) &&	\
	   ((irqnum) <= BSP_PROCESSOR_IRQ_MAX_OFFSET))
/*
 * Summary
 */
#define BSP_IRQ_NUMBER                  (BSP_PROCESSOR_IRQ_MAX_OFFSET+1)
#define BSP_LOWEST_OFFSET		BSP_SIU_IRQ_LOWEST_OFFSET
#define BSP_MAX_OFFSET                  BSP_PROCESSOR_IRQ_MAX_OFFSET

#define BSP_IS_VALID_IRQ(irqnum) 	\
          (BSP_IS_PROCESSOR_IRQ(irqnum)	\
           || BSP_IS_SIU_IRQ(irqnum)    \
           || BSP_IS_CPM_IRQ(irqnum))

#ifndef ASM
#ifdef __cplusplus
extern "C" {
#endif

/*
 * index table for the module specific handlers, a few entries are only placeholders
 */
  typedef enum {
    BSP_SIU_EXT_IRQ_0       = BSP_SIU_IRQ_LOWEST_OFFSET + 0,
    BSP_SIU_INT_IRQ_0       = BSP_SIU_IRQ_LOWEST_OFFSET + 1,
    BSP_SIU_EXT_IRQ_1       = BSP_SIU_IRQ_LOWEST_OFFSET + 2,
    BSP_SIU_INT_IRQ_1       = BSP_SIU_IRQ_LOWEST_OFFSET + 3,
    BSP_SIU_EXT_IRQ_2       = BSP_SIU_IRQ_LOWEST_OFFSET + 4,
    BSP_SIU_INT_IRQ_2       = BSP_SIU_IRQ_LOWEST_OFFSET + 5,
    BSP_SIU_EXT_IRQ_3       = BSP_SIU_IRQ_LOWEST_OFFSET + 6,
    BSP_SIU_INT_IRQ_3       = BSP_SIU_IRQ_LOWEST_OFFSET + 7,
    BSP_SIU_EXT_IRQ_4       = BSP_SIU_IRQ_LOWEST_OFFSET + 8,
    BSP_SIU_INT_IRQ_4       = BSP_SIU_IRQ_LOWEST_OFFSET + 9,
    BSP_SIU_EXT_IRQ_5       = BSP_SIU_IRQ_LOWEST_OFFSET + 10,
    BSP_SIU_INT_IRQ_5       = BSP_SIU_IRQ_LOWEST_OFFSET + 11,
    BSP_SIU_EXT_IRQ_6       = BSP_SIU_IRQ_LOWEST_OFFSET + 12,
    BSP_SIU_INT_IRQ_6       = BSP_SIU_IRQ_LOWEST_OFFSET + 13,
    BSP_SIU_EXT_IRQ_7       = BSP_SIU_IRQ_LOWEST_OFFSET + 14,
    BSP_SIU_INT_IRQ_7       = BSP_SIU_IRQ_LOWEST_OFFSET + 15,
    BSP_SIU_IRQ_LAST        = BSP_SIU_IRQ_MAX_OFFSET,
  /*
     * Some CPM IRQ symbolic name definition
     */
    BSP_CPM_IRQ_ERROR		= (BSP_CPM_IRQ_LOWEST_OFFSET),
    BSP_CPM_IRQ_PARALLEL_IO_PC4	= (BSP_CPM_IRQ_LOWEST_OFFSET + 1),
    BSP_CPM_IRQ_PARALLEL_IO_PC5	= (BSP_CPM_IRQ_LOWEST_OFFSET + 2),
    BSP_CPM_IRQ_SMC2_OR_PIP	= (BSP_CPM_IRQ_LOWEST_OFFSET + 3),
    BSP_CPM_IRQ_SMC1		= (BSP_CPM_IRQ_LOWEST_OFFSET + 4),
    BSP_CPM_IRQ_SPI		= (BSP_CPM_IRQ_LOWEST_OFFSET + 5),
    BSP_CPM_IRQ_PARALLEL_IO_PC6	= (BSP_CPM_IRQ_LOWEST_OFFSET + 6),
    BSP_CPM_IRQ_TIMER_4		= (BSP_CPM_IRQ_LOWEST_OFFSET + 7),
    BSP_CPM_IRQ_PARALLEL_IO_PC7	= (BSP_CPM_IRQ_LOWEST_OFFSET + 9),
    BSP_CPM_IRQ_PARALLEL_IO_PC8	= (BSP_CPM_IRQ_LOWEST_OFFSET + 10),
    BSP_CPM_IRQ_PARALLEL_IO_PC9	= (BSP_CPM_IRQ_LOWEST_OFFSET + 11),
    BSP_CPM_IRQ_TIMER_3		= (BSP_CPM_IRQ_LOWEST_OFFSET + 12),
    BSP_CPM_IRQ_PARALLEL_IO_PC10= (BSP_CPM_IRQ_LOWEST_OFFSET + 14),
    BSP_CPM_IRQ_PARALLEL_IO_PC11= (BSP_CPM_IRQ_LOWEST_OFFSET + 15),
    BSP_CPM_I2C			= (BSP_CPM_IRQ_LOWEST_OFFSET + 16),
    BSP_CPM_RISC_TIMER_TABLE	= (BSP_CPM_IRQ_LOWEST_OFFSET + 17),
    BSP_CPM_IRQ_TIMER_2		= (BSP_CPM_IRQ_LOWEST_OFFSET + 18),
    BSP_CPM_IDMA2		= (BSP_CPM_IRQ_LOWEST_OFFSET + 20),
    BSP_CPM_IDMA1		= (BSP_CPM_IRQ_LOWEST_OFFSET + 21),
    BSP_CPM_SDMA_CHANNEL_BUS_ERR= (BSP_CPM_IRQ_LOWEST_OFFSET + 22),
    BSP_CPM_IRQ_PARALLEL_IO_PC12= (BSP_CPM_IRQ_LOWEST_OFFSET + 23),
    BSP_CPM_IRQ_PARALLEL_IO_PC13= (BSP_CPM_IRQ_LOWEST_OFFSET + 24),
    BSP_CPM_IRQ_TIMER_1		= (BSP_CPM_IRQ_LOWEST_OFFSET + 25),
    BSP_CPM_IRQ_PARALLEL_IO_PC14= (BSP_CPM_IRQ_LOWEST_OFFSET + 26),
    BSP_CPM_IRQ_SCC4		= (BSP_CPM_IRQ_LOWEST_OFFSET + 27),
    BSP_CPM_IRQ_SCC3		= (BSP_CPM_IRQ_LOWEST_OFFSET + 28),
    BSP_CPM_IRQ_SCC2		= (BSP_CPM_IRQ_LOWEST_OFFSET + 29),
    BSP_CPM_IRQ_SCC1		= (BSP_CPM_IRQ_LOWEST_OFFSET + 30),
    BSP_CPM_IRQ_PARALLEL_IO_PC15= (BSP_CPM_IRQ_LOWEST_OFFSET + 31),
    BSP_CPM_IRQ_LAST     = BSP_CPM_IRQ_MAX_OFFSET,
  } rtems_irq_symbolic_name;

  /*
   * Symbolic name for CPM interrupt on SIU Internal level 2
   */
#define BSP_CPM_INTERRUPT	BSP_SIU_INT_IRQ_2
#define BSP_PERIODIC_TIMER	BSP_SIU_INT_IRQ_6
#define BSP_FAST_ETHERNET_CTRL	BSP_SIU_INT_IRQ_3

#define BSP_INTERRUPT_VECTOR_COUNT (BSP_MAX_OFFSET + 1)

extern int BSP_irq_enabled_at_cpm(const rtems_irq_number irqLine);

#ifdef __cplusplus
}
#endif
#endif /* ASM */

#endif /* TQM8XX_IRQ_IRQ_H */
