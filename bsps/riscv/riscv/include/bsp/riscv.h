/*
 * Copyright (c) 2018 embedded brains GmbH
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

#ifdef __cplusplus
extern "C" {
#endif

extern volatile RISCV_CLINT_regs *riscv_clint;

void *riscv_fdt_get_address(const void *fdt, int node);

uint32_t riscv_get_core_frequency(void);

#ifdef RTEMS_SMP
extern uint32_t riscv_hart_count;
#else
#define riscv_hart_count 1
#endif

uint32_t riscv_get_hart_index_by_phandle(uint32_t phandle);

#if RISCV_ENABLE_HTIF_SUPPORT != 0
void htif_poweroff(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* BSP_RISCV_H */
