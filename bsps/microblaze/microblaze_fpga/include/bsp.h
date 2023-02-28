/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief Core BSP definitions
 */

/*
 * Copyright (C) 2015 Hesham Almatary
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#ifndef LIBBSP_MICROBLAZE_FPGA_BSP_H
#define LIBBSP_MICROBLAZE_FPGA_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>
#include <bsp/default-initial-extension.h>
#include <bsp/microblaze-fdt-support.h>

#include <rtems.h>

#ifdef BSP_MICROBLAZE_FPGA_USE_FDT
#define BSP_FDT_IS_SUPPORTED
#ifndef BSP_START_COPY_FDT_FROM_U_BOOT
extern const unsigned char system_dtb[];
extern const size_t system_dtb_size;
#endif /* BSP_START_COPY_FDT_FROM_U_BOOT */
#endif /* BSP_MICROBLAZE_FPGA_USE_FDT */

void microblaze_enable_icache(void);
void microblaze_enable_dcache(void);
void microblaze_invalidate_icache(void);
void microblaze_invalidate_dcache(void);
void microblaze_invalidate_dcache_range(unsigned int cacheaddr, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_MICROBLAZE_FPGA_BSP_H */
