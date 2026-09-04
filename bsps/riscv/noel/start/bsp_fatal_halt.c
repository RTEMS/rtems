/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVNOEL
 *
 * @brief This source file contains the implementation of bsp_reset().
 */

/*
 * Copyright (C) 2021 Cobham Gaisler AB
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

#include <bsp/bootcard.h>

#include <bsp/riscv.h>

#ifdef RISCV_USE_S_MODE
#include <machine/sbi.h>
#endif

/* From bsps/arm/fvp/include/bsp/semihosting.h */
#define TARGET_SYS_EXIT_EXTENDED    0x20
#define ADP_Stopped_ApplicationExit 0x20026

/*
 * The semihosting exit takes the operation in a0 and the address of the
 * parameter block in a1.  The trap sequence must not cross a page boundary.
 */
#define SEMIHOSTING_EXIT( block )                                      \
  do {                                                                 \
    register uintptr_t  a0 __asm__( "a0" ) = TARGET_SYS_EXIT_EXTENDED; \
    register uintptr_t *a1 __asm__( "a1" ) = block;                    \
    __asm__ volatile( ".option push\n\t"                               \
                      ".option norvc\n\t"                              \
                      ".align 4\n\t"                                   \
                      "slli zero, zero, 0x1f\n\t"                      \
                      "ebreak\n\t"                                     \
                      "srai zero, zero, 0x7\n\t"                       \
                      ".option pop"                                    \
                      :                                                \
                      : "r"( a0 ), "r"( a1 )                           \
                      : "memory" );                                    \
    RTEMS_UNREACHABLE();                                               \
  } while ( 0 )

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  (void) source;

#ifdef RISCV_USE_S_MODE
  (void) code;

  sbi_system_reset( SBI_SRST_TYPE_SHUTDOWN, SBI_SRST_REASON_NONE );
  RTEMS_UNREACHABLE();
#else
  uintptr_t block[ 2 ] = { ADP_Stopped_ApplicationExit, code };

  SEMIHOSTING_EXIT( block );
#endif
}
