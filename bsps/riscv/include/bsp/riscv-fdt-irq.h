/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVShared
 *
 * @brief This header file provides the interrupt controller and timer
 *   interfaces of the shared drivers for BSPs which use a device tree.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifndef _BSP_RISCV_FDT_IRQ_H
#define _BSP_RISCV_FDT_IRQ_H

#include <bsp/fdt.h>
#include <bsp/riscv-fdt.h>

#include <rtems/score/cpuimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSBSPsRISCVShared
 *
 * @{
 */

#ifdef RTEMS_SMP
#define RISCV_FDT_PROCESSOR_MAXIMUM CPU_MAXIMUM_PROCESSORS
#else
#define RISCV_FDT_PROCESSOR_MAXIMUM 1
#endif

/**
 * @name Absent Interrupt Controller Resources
 *
 * The drivers check for an absent resource only if its define exists.
 *
 * @{
 */

#ifndef RISCV_USE_S_MODE
/**
 * @brief This define indicates that a processor may have no CLINT index.
 */
#define RISCV_CLINT_INDEX_MAY_BE_ABSENT
#endif

/**
 * @brief This define indicates that the system may have no PLIC.
 */
#define RISCV_PLIC_MAY_BE_ABSENT

/**
 * @brief This define indicates that a processor may have no PLIC context.
 */
#define RISCV_PLIC_CONTEXT_MAY_BE_ABSENT

/** @} */

#ifndef RISCV_USE_S_MODE
/**
 * @brief This pointer provides the CLINT register block of the device tree.
 */
extern volatile RISCV_CLINT_regs *riscv_clint;

/**
 * @brief This table provides the CLINT index of each processor.
 *
 * The value UINT32_MAX indicates a processor without a CLINT index.
 */
extern uint32_t riscv_fdt_clint_index[ RISCV_FDT_PROCESSOR_MAXIMUM ];
#endif

/**
 * @brief This pointer provides the PLIC register block of the device tree.
 *
 * The value NULL indicates a system without a PLIC.
 */
extern volatile RISCV_PLIC_regs *riscv_fdt_plic;

/**
 * @brief This table provides the PLIC context of each processor.
 *
 * The value UINT32_MAX indicates a processor without a PLIC context.
 */
extern uint32_t riscv_fdt_plic_context[ RISCV_FDT_PROCESSOR_MAXIMUM ];

/**
 * @brief This object provides the number of PLIC interrupt sources.
 */
extern uint32_t riscv_fdt_plic_interrupt_count;

#ifdef RISCV_USE_S_MODE
/**
 * @brief This object is true, if the processor implements the Sstc extension.
 */
extern bool riscv_fdt_has_sstc;
#endif

/**
 * @brief Initializes the objects of this header file from the device tree.
 *
 * The harts of the device tree shall be known.
 */
void riscv_fdt_irq_initialize( void );

#ifndef RISCV_USE_S_MODE
/**
 * @brief Gets the CLINT register block.
 *
 * @return Returns the CLINT register block.
 */
static inline volatile RISCV_CLINT_regs *riscv_clint_get( void )
{
  return riscv_clint;
}

/**
 * @brief Gets the CLINT index of the processor.
 *
 * @param cpu_index is the index of the processor.  It shall be less than
 *   the configured maximum number of processors, which may exceed the
 *   number of processors in the device tree.
 *
 * @return Returns the CLINT index of the processor.  The value UINT32_MAX
 *   indicates a processor without a CLINT index.
 */
static inline uint32_t riscv_clint_get_index( uint32_t cpu_index )
{
  if ( cpu_index >= RISCV_FDT_PROCESSOR_MAXIMUM ) {
    return UINT32_MAX;
  }

  return riscv_fdt_clint_index[ cpu_index ];
}
#endif

/**
 * @brief Gets the PLIC register block.
 *
 * @return Returns the PLIC register block or NULL.
 */
static inline volatile RISCV_PLIC_regs *riscv_plic_get( void )
{
  return riscv_fdt_plic;
}

/**
 * @brief Gets the PLIC context of the processor.
 *
 * @param cpu_index is the index of the processor.  It shall be less than
 *   the configured maximum number of processors, which may exceed the
 *   number of processors in the device tree.
 *
 * @return Returns the PLIC context of the processor.  The value UINT32_MAX
 *   indicates a processor without a PLIC context.
 */
static inline uint32_t riscv_plic_get_context( uint32_t cpu_index )
{
  if ( cpu_index >= RISCV_FDT_PROCESSOR_MAXIMUM ) {
    return UINT32_MAX;
  }

  return riscv_fdt_plic_context[ cpu_index ];
}

/**
 * @brief Gets the number of PLIC interrupt sources.
 *
 * @return Returns the number of PLIC interrupt sources.
 */
static inline uint32_t riscv_plic_get_interrupt_count( void )
{
  return riscv_fdt_plic_interrupt_count;
}

/**
 * @brief Gets the timebase frequency.
 *
 * @return Returns the timebase frequency in Hz.
 */
static inline uint32_t riscv_get_timebase_frequency( void )
{
  return riscv_clock_get_timebase_frequency( bsp_fdt_get() );
}

#ifdef RISCV_USE_S_MODE
/**
 * @brief Checks if the processor implements the Sstc extension.
 *
 * @return Returns true, if the processor implements the Sstc extension,
 *   otherwise false.
 */
static inline bool riscv_has_sstc_extension( void )
{
  return riscv_fdt_has_sstc;
}
#endif

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BSP_RISCV_FDT_IRQ_H */
