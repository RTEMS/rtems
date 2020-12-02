/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2021 Jan Sommer, German Aerospace Center (DLR)
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

#ifndef LIBBSP_ARM_XILINX_ZYNQ_CADENCE_SPI_REGS_H
#define LIBBSP_ARM_XILINX_ZYNQ_CADENCE_SPI_REGS_H

#include <bsp/utility.h>

typedef struct {
    uint32_t config;
#define CADENCE_SPI_CONFIG_MODEFAIL_EN BSP_BIT32(17)
#define CADENCE_SPI_CONFIG_MANSTRT BSP_BIT32(16)
#define CADENCE_SPI_CONFIG_MANSTRT_EN BSP_BIT32(15)
#define CADENCE_SPI_CONFIG_MANUAL_CS BSP_BIT32(14)
#define CADENCE_SPI_CONFIG_CS(val) BSP_FLD32(val, 10, 13)
#define CADENCE_SPI_CONFIG_CS_GET(reg) BSP_FLD32GET(reg, 10, 13)
#define CADENCE_SPI_CONFIG_CS_SET(reg, val) BSP_FLD32SET(reg, val, 10, 13)
#define CADENCE_SPI_CONFIG_PERI_SEL BSP_BIT32(9)
#define CADENCE_SPI_CONFIG_REF_CLK BSP_BIT32(8)
#define CADENCE_SPI_CONFIG_BAUD_DIV(val) BSP_FLD32(val, 3, 5)
#define CADENCE_SPI_CONFIG_BAUD_DIV_GET(reg) BSP_FLD32GET(reg, 3, 5)
#define CADENCE_SPI_CONFIG_BAUD_DIV_SET(reg, val) BSP_FLD32SET(reg, val, 3, 5)
#define CADENCE_SPI_CONFIG_CLK_PH BSP_BIT32(2)
#define CADENCE_SPI_CONFIG_CLK_POL BSP_BIT32(1)
#define CADENCE_SPI_CONFIG_MSTREN BSP_BIT32(0)
	uint32_t irqstatus;
	uint32_t irqenable;
	uint32_t irqdisable;
	uint32_t irqmask;
#define CADENCE_SPI_IXR_TXUF BSP_BIT32(6)
#define CADENCE_SPI_IXR_RXFULL BSP_BIT32(5)
#define CADENCE_SPI_IXR_RXNEMPTY BSP_BIT32(4)
#define CADENCE_SPI_IXR_TXFULL BSP_BIT32(3)
#define CADENCE_SPI_IXR_TXOW BSP_BIT32(2)
#define CADENCE_SPI_IXR_MODF BSP_BIT32(1)
#define CADENCE_SPI_IXR_RXOVR BSP_BIT32(0)
    uint32_t spienable;
#define CADENCE_SPI_EN BSP_BIT32(0)
    uint32_t delay;
#define CADENCE_SPI_DELAY_DNSS(val) BSP_FLD32(val, 24, 31)
#define CADENCE_SPI_DELAY_DNSS_GET(reg) BSP_FLD32GET(reg, 24, 31)
#define CADENCE_SPI_DELAY_DNSS_SET(reg, val) BSP_FLD32SET(reg, val, 24, 31)
#define CADENCE_SPI_DELAY_DBTWN(val) BSP_FLD32(val, 16, 23)
#define CADENCE_SPI_DELAY_DBTWN_GET(reg) BSP_FLD32GET(reg, 16, 23)
#define CADENCE_SPI_DELAY_DBTWN_SET(reg, val) BSP_FLD32SET(reg, val, 16, 23)
#define CADENCE_SPI_DELAY_DAFTER(val) BSP_FLD32(val, 8, 15)
#define CADENCE_SPI_DELAY_DAFTER_GET(reg) BSP_FLD32GET(reg, 8, 15)
#define CADENCE_SPI_DELAY_DAFTER_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
#define CADENCE_SPI_DELAY_DINT(val) BSP_FLD32(val, 0, 7)
#define CADENCE_SPI_DELAY_DINT_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define CADENCE_SPI_DELAY_DINT_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
	uint32_t txdata;
	uint32_t rxdata;
	uint32_t slave_idle_count;
	uint32_t txthreshold;
	uint32_t rxthreshold;
	uint32_t moduleid;
} cadence_spi;

#endif /* LIBBSP_ARM_XILINX_ZYNQ_CADENCE_SPI_REGS_H */
