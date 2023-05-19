/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief Register definitions.
 */

/*
 * Copyright (C) 2008, 2014 embedded brains GmbH & Co. KG
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

#ifndef LIBCPU_POWERPC_MPC55XX_REG_DEFS_H
#define LIBCPU_POWERPC_MPC55XX_REG_DEFS_H

#include <bspopts.h>

#if MPC55XX_CHIP_FAMILY == 551
  #define FLASH_BIUCR   0xFFFF801C
#else
  #define FLASH_BIUCR 0xC3F8801C
#endif

/*
 * Definitions for FLASH_BIUCR (Flash BIU Control Register)
 */

/* Fields for Flash Bus Interface Control */
/* Fields for Prefetch Control (MnPFE Master n Prefetch Enable) */

/* Fields for M3PFE (Master 3 (EBI) prefetch enable bit [12]) */
#define FLASH_BUICR_EBI_PREFTCH 0x00080000

/* Fields for M2PFE (Master 2 (eDMA) prefetch enable bit [13]) */
#define FLASH_BUICR_EDMA_PREFTCH 0x00040000

/* Fields for M1PFE (Master 1 (Nexus) prefetch enable bit [14]) */
#define FLASH_BUICR_NEX_PREFTCH 0x00020000

/* Fields for M0PFE (Master 0 (e200z core) prefetch enable bit [15]) */
#define FLASH_BUICR_CPU_PREFTCH 0x00010000

/* Fields for APC (access pipelining control bits [16:18]) */
#define FLASH_BUICR_APC_0 0x00000000
#define FLASH_BUICR_APC_1 0x00002000
#define FLASH_BUICR_APC_2 0x00004000
#define FLASH_BUICR_APC_3 0x00006000
#define FLASH_BUICR_APC_4 0x00008000
#define FLASH_BUICR_APC_5 0x0000A000
#define FLASH_BUICR_APC_6 0x0000C000
#define FLASH_BUICR_APC_NO 0x0000E000

/* Fields for WWSC (write wait state control bits [19:20]) */
#define FLASH_BUICR_WWSC_1 0x00000800
#define FLASH_BUICR_WWSC_2 0x00001000
#define FLASH_BUICR_WWSC_3 0x00001800

/* Fields for RWSC (read wait state control bits [21:23]) */
#define FLASH_BUICR_RWSC_0 0x00000000
#define FLASH_BUICR_RWSC_1 0x00000100
#define FLASH_BUICR_RWSC_2 0x00000200
#define FLASH_BUICR_RWSC_3 0x00000300
#define FLASH_BUICR_RWSC_4 0x00000400
#define FLASH_BUICR_RWSC_5 0x00000500
#define FLASH_BUICR_RWSC_6 0x00000600
#define FLASH_BUICR_RWSC_7 0x00000700

/* Fields for DPFEN (data prefetch enable bits [24:25]) */
#define FLASH_BUICR_DPFEN_0 0x00000000
#define FLASH_BUICR_DPFEN_1 0x00000040
#define FLASH_BUICR_DPFEN_3 0x000000C0

/* Fields for IPFEN (instruction prefetch enable bits [26:27]) */
#define FLASH_BUICR_IPFEN_0 0x00000000
#define FLASH_BUICR_IPFEN_1 0x00000010
#define FLASH_BUICR_IPFEN_3 0x00000030

/* Fields for PFLIM (additional line prefetch (limit) bits [28:30]) */
#define FLASH_BUICR_PFLIM_0 0x00000000
#define FLASH_BUICR_PFLIM_1 0x00000002
#define FLASH_BUICR_PFLIM_2 0x00000004
#define FLASH_BUICR_PFLIM_3 0x00000006
#define FLASH_BUICR_PFLIM_4 0x00000008
#define FLASH_BUICR_PFLIM_5 0x0000000A
#define FLASH_BUICR_PFLIM_6 0x0000000C

/* Fields for BFEN (enable line read buffer hits bit [31]) */
#define FLASH_BUICR_BFEN 0x00000001

#endif /* LIBCPU_POWERPC_MPC55XX_REG_DEFS_H */
