/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This header file provides interfaces of the Parameter Overlay Module
 *   (POM) support.
 */

/*
 * Copyright (C) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#ifndef LIBBSP_ARM_TMS570_POM_H
#define LIBBSP_ARM_TMS570_POM_H

#include <stdint.h>
#include <bsp/tms570.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TMS570_POM_REGIONS 32
#define TMS570_POM_GLBCTRL_ENABLE 0x000000a0a

/* Specification of memory size used for field REGSIZE of tms570_pom_region_t */
#define TMS570_POM_REGSIZE_DISABLED 0x0
#define TMS570_POM_REGSIZE_64B      0x1
#define TMS570_POM_REGSIZE_128B     0x2
#define TMS570_POM_REGSIZE_256B     0x3
#define TMS570_POM_REGSIZE_512B     0x4
#define TMS570_POM_REGSIZE_1KB      0x5
#define TMS570_POM_REGSIZE_2KB      0x6
#define TMS570_POM_REGSIZE_4KB      0x7
#define TMS570_POM_REGSIZE_8KB      0x8
#define TMS570_POM_REGSIZE_16KB     0x9
#define TMS570_POM_REGSIZE_32KB     0xa
#define TMS570_POM_REGSIZE_64KB     0xb
#define TMS570_POM_REGSIZE_128KB    0xc
#define TMS570_POM_REGSIZE_256KB    0xd

#define TMS570_POM_REGADDRMASK    ((1<<23)-1)

void tms570_pom_initialize_and_clear(void);
void tms570_pom_remap(void);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_TMS570_POM_H */
