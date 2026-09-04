/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVNOEL
 *
 * @brief This header file provides the CPU counter, the IPI and the debug UART
 *   interfaces of the NOEL-V BSP.
 */

/*
 * Copyright (c) 2018 embedded brains GmbH & Co. KG
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef BSP_RISCV_H
#define BSP_RISCV_H

#include <bsp.h>

#include <rtems/score/cpuimpl.h>
#include <grlib/apbuart-regs.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t riscv_up_counter_low( void )
{
  unsigned long counter;
  __asm__ volatile( ".option push\n"
                    ".option arch, +zicsr\n"
                    "csrr %0, time\n"
                    ".option pop"
                    : "=r"( counter ) );

  return (uint32_t) ( counter & __UINT32_MAX__ );
}

extern volatile RISCV_CLINT_regs *noel_clint;

void riscv_send_ipi( uint32_t cpu_index );

/**
 * @brief This pointer provides the debug APBUART register block address.
 */
#if defined( GRLIB_APBUART_BASE )
#define grlib_debug_uart ( (struct apbuart *) GRLIB_APBUART_BASE )
#else
extern apbuart *grlib_debug_uart;
#endif

#ifdef __cplusplus
}
#endif

#endif /* BSP_RISCV_H */
