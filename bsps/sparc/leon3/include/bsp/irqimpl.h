/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This header file provides interfaces used by the interrupt support
 *   implementation.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_SPARC_LEON3_BSP_IRQIMPL_H
#define LIBBSP_SPARC_LEON3_BSP_IRQIMPL_H

#include <rtems.h>
#include <grlib/grlib.h>

struct ambapp_dev;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSBSPsSPARCLEON3
 *
 * @{
 */

/**
 * @brief This lock serializes the interrupt controller access.
 */
extern rtems_interrupt_lock LEON3_IrqCtrl_Lock;

/**
 * @brief This pointer provides the IRQ(A)MP register block address.
 */
extern volatile struct irqmp_regs *LEON3_IrqCtrl_Regs;

/**
 * @brief This pointer provides the IRQ(A)MP device information block.
 */
extern struct ambapp_dev *LEON3_IrqCtrl_Adev;

/**
 * @brief This object provides the interrupt number used to multiplex extended
 *   interrupts or is zero if no extended interrupts are available.
 *
 * This object should be read-only after initialization.
 */
extern uint32_t LEON3_IrqCtrl_EIrq;

/**
 * @brief Initializes the interrupt controller for the boot processor.
 *
 * @param[in, out] regs is the IRQ(A)MP register block address.
 */
void leon3_ext_irq_init( volatile struct irqmp_regs *regs );

/**
 * @brief Acknowledges and maps extended interrupts if this feature is
 * available and the interrupt for extended interrupts is present.
 *
 * @param irq is the standard interrupt number.
 */
static inline uint32_t bsp_irq_fixup( uint32_t irq )
{
  uint32_t eirq;

  if ( irq != LEON3_IrqCtrl_EIrq ) {
    return irq;
  }

  eirq = LEON3_IrqCtrl_Regs->intid[ _LEON3_Get_current_processor() ] & 0x1f;

  if ( eirq < 16 ) {
    return irq;
  }

  return eirq;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_SPARC_LEON3_BSP_IRQIMPL_H */
