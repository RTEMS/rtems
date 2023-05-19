/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS virtex BSP
 *
 * This file declares constants of the interrupt controller.
 */

/*
 * Copyright (c) 2007 embedded brains GmbH & Co. KG
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

#ifndef VIRTEX5_IRQ_IRQ_H
#define VIRTEX5_IRQ_IRQ_H

#include <rtems/irq.h>
#include <bsp/irq-default.h>

/*
 * the following definitions specify the indices used
 * to interface the interrupt handler API
 */

/*
 * Peripheral IRQ handlers related definitions
 */
  /* Not supported at this level */

/*
 * Processor IRQ handlers related definitions
 */
#define BSP_PROCESSOR_IRQ_NUMBER        3
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET 0
#define BSP_PROCESSOR_IRQ_MAX_OFFSET    (BSP_PROCESSOR_IRQ_LOWEST_OFFSET\
                                         +BSP_PROCESSOR_IRQ_NUMBER-1)

#define BSP_IS_PROCESSOR_IRQ(irqnum)                 \
  (((irqnum) >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET) &&  \
   ((irqnum) <= BSP_PROCESSOR_IRQ_MAX_OFFSET))

/*
 * Summary
 */
#define BSP_IRQ_NUMBER                  (BSP_PROCESSOR_IRQ_MAX_OFFSET+1)
#define BSP_LOWEST_OFFSET                BSP_PROCESSOR_IRQ_LOWEST_OFFSET
#define BSP_MAX_OFFSET                   BSP_PROCESSOR_IRQ_MAX_OFFSET

#define BSP_IS_VALID_IRQ(irqnum)        (BSP_IS_PROCESSOR_IRQ(irqnum))

#ifndef ASM
#ifdef __cplusplus
extern "C" {
#endif

/*
 * index table for the module specific handlers, a few entries are only placeholders
 */
  typedef enum {
    BSP_EXT               = BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 0,
    BSP_PIT               = BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 1,
    BSP_CRIT              = BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 2
  } rtems_irq_symbolic_name;

  extern rtems_irq_connect_data *BSP_rtems_irq_tbl;
  void BSP_irqexc_on_fnc(const rtems_irq_connect_data *conn_data);
  void BSP_irqexc_off_fnc(const rtems_irq_connect_data *unused);
  void BSP_rtems_irq_mngt_init(unsigned cpuId);

#define	BSP_DEC           BSP_PIT
#define	BSP_DECREMENTER   BSP_PIT

#ifdef __cplusplus
}
#endif
#endif /* ASM */

#endif /* VIRTEX5_IRQ_IRQ_H */
