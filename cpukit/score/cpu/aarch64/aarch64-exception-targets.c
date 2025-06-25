/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief This source file contains the jump targets for AArch64 exceptions.
 */

/*
 * Copyright (C) 2025 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <stdint.h>
#include <rtems/score/cpu.h>

void aarch64_exception_dump_sp0(void);
void aarch64_exception_dump_spx(void);

AArch64_Exception_Handler aarch64_exception_sp0_synchronous =
  (AArch64_Exception_Handler) &_AArch64_Exception_default;
AArch64_Exception_Handler aarch64_exception_sp0_irq =
  &aarch64_exception_dump_sp0;
AArch64_Exception_Handler aarch64_exception_sp0_fiq =
  &aarch64_exception_dump_sp0;
AArch64_Exception_Handler aarch64_exception_sp0_serror =
  &aarch64_exception_dump_sp0;
AArch64_Exception_Handler aarch64_exception_spx_synchronous =
  (AArch64_Exception_Handler) &_AArch64_Exception_default;
AArch64_Exception_Handler aarch64_exception_spx_irq =
  &aarch64_exception_dump_spx;
AArch64_Exception_Handler aarch64_exception_spx_fiq =
  &aarch64_exception_dump_spx;
AArch64_Exception_Handler aarch64_exception_spx_serror =
  &aarch64_exception_dump_spx;
AArch64_Exception_Handler aarch64_exception_lel64_synchronous =
  &aarch64_exception_dump_spx;
AArch64_Exception_Handler aarch64_exception_lel64_irq =
  &aarch64_exception_dump_spx;
AArch64_Exception_Handler aarch64_exception_lel64_fiq =
  &aarch64_exception_dump_spx;
AArch64_Exception_Handler aarch64_exception_lel64_serror =
  &aarch64_exception_dump_spx;
AArch64_Exception_Handler aarch64_exception_lel32_synchronous =
  &aarch64_exception_dump_spx;
AArch64_Exception_Handler aarch64_exception_lel32_irq =
  &aarch64_exception_dump_spx;
AArch64_Exception_Handler aarch64_exception_lel32_fiq =
  &aarch64_exception_dump_spx;
AArch64_Exception_Handler aarch64_exception_lel32_serror =
  &aarch64_exception_dump_spx;
