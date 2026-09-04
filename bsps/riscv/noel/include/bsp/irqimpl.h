/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVNOEL
 *
 * @brief This header file provides the interfaces used by the shared
 *   interrupt controller and clock drivers.
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

#ifndef LIBBSP_RISCV_NOEL_BSP_IRQIMPL_H
#define LIBBSP_RISCV_NOEL_BSP_IRQIMPL_H

#include <amba.h>
#include <bsp/riscv.h>

#include <rtems/score/cpuimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSBSPsRISCVNOEL
 *
 * @{
 */

/**
 * @brief This pointer provides the PLIC register block found through AMBA
 *   Plug & Play.
 *
 * The value NULL indicates a system without a PLIC in use.
 */
extern volatile RISCV_PLIC_regs *noel_plic;

/**
 * @brief Finds the CLINT and the PLIC through AMBA Plug & Play.
 */
void noel_irq_initialize( void );

#ifndef RISCV_USE_S_MODE
/**
 * @brief Gets the CLINT register block.
 *
 * @return Returns the CLINT register block.
 */
static inline volatile RISCV_CLINT_regs *riscv_clint_get( void )
{
  return noel_clint;
}

/**
 * @brief Gets the CLINT index of the processor.
 *
 * @param cpu_index is the index of the processor.
 *
 * @return Returns the CLINT index of the processor.
 */
static inline uint32_t riscv_clint_get_index( uint32_t cpu_index )
{
  return _RISCV_Map_cpu_index_to_hartid( cpu_index );
}
#endif

/**
 * @brief This define indicates that the system may have no PLIC.
 */
#define RISCV_PLIC_MAY_BE_ABSENT

/**
 * @brief Gets the PLIC register block.
 *
 * @return Returns the PLIC register block or NULL.
 */
static inline volatile RISCV_PLIC_regs *riscv_plic_get( void )
{
  return noel_plic;
}

/**
 * @brief Gets the PLIC context of the processor.
 *
 * @param cpu_index is the index of the processor.
 *
 * @return Returns the PLIC context of the processor.
 */
static inline uint32_t riscv_plic_get_context( uint32_t cpu_index )
{
  /*
   * The GRPLIC provides four contexts for each hart.  The first one serves
   * the machine mode.
   */
  return 4 * _RISCV_Map_cpu_index_to_hartid( cpu_index );
}

/**
 * @brief Gets the number of PLIC interrupt sources.
 *
 * @return Returns the number of PLIC interrupt sources.
 */
static inline uint32_t riscv_plic_get_interrupt_count( void )
{
#ifdef RISCV_USE_S_MODE
  return 0;
#else
  return RISCV_MAXIMUM_EXTERNAL_INTERRUPTS;
#endif
}

/**
 * @brief Gets the timebase frequency.
 *
 * @return Returns the timebase frequency in Hz.
 */
static inline uint32_t riscv_get_timebase_frequency( void )
{
  return grlib_up_counter_frequency();
}

#ifdef RISCV_USE_S_MODE
/**
 * @brief Checks if the processor implements the Sstc extension.
 *
 * @return Returns true.
 */
static inline bool riscv_has_sstc_extension( void )
{
  /*
   * The NOEL-V standard configurations HP and GP provide the supervisor mode.
   * Both implement the Sstc extension.
   */
  return true;
}
#endif

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_RISCV_NOEL_BSP_IRQIMPL_H */
