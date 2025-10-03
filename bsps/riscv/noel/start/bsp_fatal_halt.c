/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2021 Cobham Gaisler AB
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

#include <bsp/bootcard.h>

#include <bsp/riscv.h>

/* From bsps/arm/fvp/include/bsp/semihosting.h */
#define TARGET_SYS_EXIT_EXTENDED 0x20
#define ADP_Stopped_ApplicationExit 0x20026

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  uint64_t args[2] = {ADP_Stopped_ApplicationExit, code};
  __asm__ volatile ("li a0, %0" ::"i"(TARGET_SYS_EXIT_EXTENDED));
  __asm__ volatile ("mv a1, %0" ::"r"(&args));
  __asm__ volatile (".align 4; slli zero, zero, 0x1f");
  __asm__ volatile ("ebreak");
  __asm__ volatile ("srai zero, zero, 0x7");
  RTEMS_UNREACHABLE();
}
