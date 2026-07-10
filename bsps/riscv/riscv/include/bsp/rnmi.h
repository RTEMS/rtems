/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVGeneric
 *
 * @brief RISC-V BSP specific RNMI definitions.
 */

/*
 * Copyright (C) 2025-2026 Intel Corporation
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

#ifndef BSP_RNMI_H
#define BSP_RNMI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* QEMU puts the RNMI line number, 0 to 15, in the low bits of mncause */

/**
 * @brief Deferred RNMI handler
 *
 * Called from _RISCV_RNMI_deferred. Weak default implementation, override by application
 *
 * @param dropped Number of RNMIs this processor ignored since the previous
 *   call, because the deferred work of an RNMI was pending.
 */
void riscv_rnmi_deferred_work_dispatch(
  uintptr_t mncause,
  uintptr_t mnepc,
  uintptr_t dropped
);

/**
 * @brief Enable deferred RNMI processing on all online cores
 */
void riscv_rnmi_deferred_enable(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_RNMI_H */
