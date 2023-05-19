/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup RTEMSBSPsARMShared
 *
 *  @brief ARM PL050 Register Definitions
 */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_SHARED_ARM_PL050_REGS_H
#define LIBBSP_ARM_SHARED_ARM_PL050_REGS_H

#include <bsp/utility.h>

typedef struct {
	uint32_t kmicr;
#define PL050_KMICR_KMITYPE BSP_BIT32(5)
#define PL050_KMICR_KMIRXINTREN BSP_BIT32(4)
#define PL050_KMICR_KMITXINTREN BSP_BIT32(3)
#define PL050_KMICR_KMIEN BSP_BIT32(2)
#define PL050_KMICR_FKMID BSP_BIT32(1)
#define PL050_KMICR_FKMIC BSP_BIT32(0)
	uint32_t kmistat;
#define PL050_KMISTAT_TXEMPTY BSP_BIT32(6)
#define PL050_KMISTAT_TXBUSY BSP_BIT32(5)
#define PL050_KMISTAT_RXFULL BSP_BIT32(4)
#define PL050_KMISTAT_RXBUSY BSP_BIT32(3)
#define PL050_KMISTAT_RXPARITY BSP_BIT32(2)
#define PL050_KMISTAT_KMIC BSP_BIT32(1)
#define PL050_KMISTAT_KMID BSP_BIT32(0)
	uint32_t kmidata;
#define PL050_KMIDATA_KMIDATA(val) BSP_FLD32(val, 0, 7)
#define PL050_KMIDATA_KMIDATA_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define PL050_KMIDATA_KMIDATA_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
	uint32_t kmiclkdiv;
#define PL050_KMICLKDIV_KMICLKDIV(val) BSP_FLD32(val, 0, 3)
#define PL050_KMICLKDIV_KMICLKDIV_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define PL050_KMICLKDIV_KMICLKDIV_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
	uint32_t kmiir;
#define PL050_KMIIR_KMITXINTR BSP_BIT32(1)
#define PL050_KMIIR_KMIRXINTR BSP_BIT32(0)
} pl050;

#endif /* LIBBSP_ARM_SHARED_ARM_PL050_REGS_H */
