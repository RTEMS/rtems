/* SPDX-License-Identifier: BSD-2-Clause */

/* Copyright (c) 2015 Hesham Almatary <hesham@alumni.york.ac.uk>
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

/**
 * @file
 *
 * @brief RISC-V utility helpers.
 */

#ifndef _RTEMS_SCORE_RISCV_UTILITY_H
#define _RTEMS_SCORE_RISCV_UTILITY_H

#include "encoding.h"

#if __riscv_xlen == 64
#define SATP_MODE_SHIFT (60)
#else
#define SATP_MODE_SHIFT (31)
#endif

#ifndef ASM

#define _RISCV_FENCE( pred, succ )                              \
  __asm__ volatile ( "fence " #pred ", " #succ : : : "memory" )

static inline void _RISCV_data_barrier( void )
{
  _RISCV_FENCE( rw, rw );
}

static inline void _RISCV_MMIO_store_release_fence( void )
{
  _RISCV_FENCE( o, i );
}

#endif /* ASM */

#endif
