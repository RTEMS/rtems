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
#include <grlib/irqamp-regs.h>
#include <grlib/io.h>

#include <bspopts.h>
#include <bsp/irq.h>

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
 * @brief This object provides the index of the boot processor.
 *
 * This object should be read-only after initialization.
 */
extern uint32_t LEON3_Cpu_Index;

/**
 * @brief This lock serializes the interrupt controller access.
 */
extern rtems_interrupt_lock LEON3_IrqCtrl_Lock;

/**
 * @brief Acquires the interrupt controller lock.
 *
 * @param[out] _lock_context is the lock context.
 */
#define LEON3_IRQCTRL_ACQUIRE( _lock_context ) \
  rtems_interrupt_lock_acquire( &LEON3_IrqCtrl_Lock, _lock_context )

/**
 * @brief Releases the interrupt controller lock.
 *
 * @param[in, out] _lock_context is the lock context.
 */
#define LEON3_IRQCTRL_RELEASE( _lock_context ) \
  rtems_interrupt_lock_release( &LEON3_IrqCtrl_Lock, _lock_context )

/**
 * @brief This pointer provides the IRQ(A)MP register block address.
 */
#if defined( LEON3_IRQAMP_BASE )
#define LEON3_IrqCtrl_Regs ( (irqamp *) LEON3_IRQAMP_BASE )
#else
extern irqamp *LEON3_IrqCtrl_Regs;

/**
 * @brief This pointer provides the IRQ(A)MP device information block.
 */
extern struct ambapp_dev *LEON3_IrqCtrl_Adev;
#endif

/**
 * @brief This object provides the interrupt number used to multiplex extended
 *   interrupts or is zero if no extended interrupts are available.
 *
 * This object should be read-only after initialization.
 */
#if defined( LEON3_IRQAMP_EXTENDED_INTERRUPT )
#define LEON3_IrqCtrl_EIrq LEON3_IRQAMP_EXTENDED_INTERRUPT
#else
extern uint32_t LEON3_IrqCtrl_EIrq;
#endif

/**
 * @brief Initializes the interrupt controller for the boot processor.
 *
 * The function sets the processor interrupt mask and the processor interrupt
 * force register of the boot processor to zero.  It writes all ones to the
 * interrupt clear register, which clears every pending interrupt.  Where the
 * BSP uses the interrupt map, the function copies the interrupt map entry of
 * each bus line into the interrupt map copy of the BSP.  It masks each entry
 * by the count of controller lines.  In SMP and multiprocessing
 * configurations, it maps bus line 0 to the controller line defined by the
 * BSP option LEON3_IPI_CONTROLLER_LINE.  Where the BSP determines the
 * extended controller line at run time, the function reads it from the
 * interrupt controller.
 *
 * @param[in, out] regs is the register block address of the first IRQ(A)MP
 *   interrupt controller.  Only the first controller provides the interrupt
 *   map registers.
 */
void leon3_ext_irq_init( irqamp *regs );

/**
 * @brief Acknowledges and maps extended interrupts if this feature is
 * available and the interrupt for extended interrupts is present.
 *
 * @param irq is the standard interrupt number.
 */
static inline uint32_t bsp_irq_fixup( uint32_t irq )
{
  uint32_t eirq;
  uint32_t cpu_self;

  if ( irq != LEON3_IrqCtrl_EIrq ) {
    return irq;
  }

  cpu_self = _LEON3_Get_current_processor();
  eirq = grlib_load_32( &LEON3_IrqCtrl_Regs->pextack[ cpu_self ] );
  eirq = IRQAMP_PEXTACK_EID_4_0_GET( eirq );

  if ( eirq < 16 ) {
    return irq;
  }

  return eirq;
}

/**
 * @brief Gets the controller line mapped to a bus line.
 *
 * Where the BSP uses no interrupt map, a bus line maps to the controller line
 * of the same number.
 *
 * This function does not perform any validation on the bus line number and
 * should only be used when the caller is certain that it is valid.
 *
 * @param bus_line is the bus line number.
 * @return the controller line to which the bus line is mapped.
 */
static inline rtems_vector_number leon3_irqmap_get_unchecked(
  rtems_vector_number bus_line
)
{
#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  return LEON3_IrqCtrl_Mapping[ bus_line ];
#else
  return bus_line;
#endif
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_SPARC_LEON3_BSP_IRQIMPL_H */
