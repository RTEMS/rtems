/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief Definitions used in MMU setup.
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#ifndef LIBCPU_AARCH64_MMU_VMSAV8_64_H
#define LIBCPU_AARCH64_MMU_VMSAV8_64_H

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <rtems.h>

/* VMSAv8 Long-descriptor fields */
#define MMU_DESC_AF                        ( 1 << 10 )
#define MMU_DESC_SH_INNER                  ( ( 1 << 9 ) | ( 1 << 8 ) )
#define MMU_DESC_WRITE_DISABLE             ( 1 << 7 )
/* PAGE and TABLE flags are the same bit, but only apply on certain levels */
#define MMU_DESC_TYPE_TABLE                ( 1 << 1 )
#define MMU_DESC_TYPE_PAGE                 ( 1 << 1 )
#define MMU_DESC_VALID                     ( 1 << 0 )
#define MMU_DESC_MAIR_ATTR( val )          ( ( val & 0x3 ) << 2 )
#define MMU_DESC_PAGE_TABLE_MASK           0xFFFFFFFFF000LL

/* Page table configuration */
#define MMU_PAGE_BITS           12
#define MMU_PAGE_SIZE           ( 1 << MMU_PAGE_BITS )
#define MMU_BITS_PER_LEVEL      9

#define AARCH64_MMU_FLAGS_BASE \
  ( MMU_DESC_VALID | MMU_DESC_SH_INNER | MMU_DESC_AF )

#define AARCH64_MMU_DATA_RO_CACHED \
  ( AARCH64_MMU_FLAGS_BASE | MMU_DESC_MAIR_ATTR( 3 ) | MMU_DESC_WRITE_DISABLE )
#define AARCH64_MMU_CODE_CACHED AARCH64_MMU_DATA_RO_CACHED
#define AARCH64_MMU_CODE_RW_CACHED AARCH64_MMU_DATA_RW_CACHED

#define AARCH64_MMU_DATA_RO \
  ( AARCH64_MMU_FLAGS_BASE | MMU_DESC_MAIR_ATTR( 2 ) | MMU_DESC_WRITE_DISABLE )
#define AARCH64_MMU_CODE AARCH64_MMU_DATA_RO
#define AARCH64_MMU_CODE_RW AARCH64_MMU_DATA_RW

/* RW implied by not ORing in RO */
#define AARCH64_MMU_DATA_RW_CACHED \
  ( AARCH64_MMU_FLAGS_BASE | MMU_DESC_MAIR_ATTR( 3 ) )
#define AARCH64_MMU_DATA_RW \
  ( AARCH64_MMU_FLAGS_BASE | MMU_DESC_MAIR_ATTR( 2 ) )
#define AARCH64_MMU_DEVICE ( AARCH64_MMU_FLAGS_BASE | MMU_DESC_MAIR_ATTR( 0 ) )

rtems_status_code aarch64_mmu_map(
  uintptr_t addr,
  uint64_t  size,
  uint64_t  flags
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBCPU_AARCH64_MMU_VMSAV8_64_H */
