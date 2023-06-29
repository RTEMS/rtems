/**
 * @file
 * @ingroup RTEMSBSPsShared
 * @brief Xilinx Zynq Ultrascale+ MPSoC Peripheral memory map.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2023 Reflex Aerospace GmbH
 *
 * Written by Philip Kirkpatrick <p.kirkpatrick@reflexaerospace.com>
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

#ifndef LIBBSP_SHARED_PERIPHERAL_MAPS_ZYNQMP
#define LIBBSP_SHARED_PERIPHERAL_MAPS_ZYNQMP

/* Data derived from https://docs.xilinx.com/r/en-US/ug1085-zynq-ultrascale-trm/PS-I/O-Peripherals-Registers */

/* LPD IO Peripherals */
#define ZYNQMP_UART0 (0xFF000000)
#define ZYNQMP_UART1 (0xFF010000)
#define ZYNQMP_I2C0 (0xFF020000)
#define ZYNQMP_I2C1 (0xFF030000)
#define ZYNQMP_SPI0 (0xFF040000)
#define ZYNQMP_SPI1 (0xFF050000)
#define ZYNQMP_CAN0 (0xFF060000)
#define ZYNQMP_CAN1 (0xFF070000)
#define ZYNQMP_GPIO (0xFF0A0000)
#define ZYNQMP_GEM0 (0xFF0B0000)
#define ZYNQMP_GEM1 (0xFF0C0000)
#define ZYNQMP_GEM2 (0xFF0D0000)
#define ZYNQMP_GEM3 (0xFF0E0000)
#define ZYNQMP_QSPI (0xFF0F0000)
#define ZYNQMP_NAND (0xFF100000)
#define ZYNQMP_SD0 (0xFF160000)
#define ZYNQMP_SD1 (0xFF170000)
#define ZYNQMP_IPI_MSG (0xFF990000)
#define ZYNQMP_USB0 (0xFF9D0000)
#define ZYNQMP_USB1 (0xFF9E0000)
#define ZYNQMP_AMS (0xFFA50000)
#define ZYNQMP_PSSYSMON (0xFFA50800)
#define ZYNQMP_PLSYSMON (0xFFA50C00)
#define ZYNQMP_CSU_SWDT (0xFFCB0000)

/* FPD IO Peripherals */
#define ZYNQMP_SATA (0xFD0C0000)
#define ZYNQMP_PCIE (0xFD0E0000)
#define ZYNQMP_PCIE_IN (0xFD0E0800)
#define ZYNQMP_PCIE_EG (0xFD0E0C00)
#define ZYNQMP_PCIE_DMA (0xFD0F0000)
#define ZYNQMP_SIOU (0xFD3D0000)
#define ZYNQMP_GTR (0xFD400000)
#define ZYNQMP_PCIE_ATTR (0xFD480000)
#define ZYNQMP_DP (0xFD4A0000)
#define ZYNQMP_GPU (0xFD4B0000)
#define ZYNQMP_DP_DMA (0xFD4C0000)

/* LPD System Registers */
#define ZYNQMP_IPI (0xFF300000)
#define ZYNQMP_TTC0 (0xFF110000)
#define ZYNQMP_TTC1 (0xFF120000)
#define ZYNQMP_TTC2 (0xFF130000)
#define ZYNQMP_TTC3 (0xFF140000)
#define ZYNQMP_LPD_SWDT (0xFF150000)
#define ZYNQMP_XPPU (0xFF980000)
#define ZYNQMP_XPPU_SINK (0xFF9C0000)
#define ZYNQMP_PL_LPD (0xFF9B0000)
#define ZYNQMP_OCM (0xFFA00000)
#define ZYNQMP_LPD_FPD (0xFFA10000)
#define ZYNQMP_RTC (0xFFA60000)
#define ZYNQMP_OCM_XMPU (0xFFA70000)
#define ZYNQMP_LPD_DMA (0xFFA80000)
#define ZYNQMP_CSU_DMA (0xFFC80000)
#define ZYNQMP_CSU (0xFFCA0000)
#define ZYNQMP_BBRAM (0xFFCD0000)

/* System Interrupt Table */

/* SPIs */
#define ZYNQMP_IRQ_UART_0 53
#define ZYNQMP_IRQ_UART_1 54

#define ZYNQMP_IRQ_TTC_0_0 68
#define ZYNQMP_IRQ_TTC_0_1 69
#define ZYNQMP_IRQ_TTC_0_2 70
#define ZYNQMP_IRQ_TTC_1_0 71
#define ZYNQMP_IRQ_TTC_1_1 72
#define ZYNQMP_IRQ_TTC_1_2 73
#define ZYNQMP_IRQ_TTC_2_0 74
#define ZYNQMP_IRQ_TTC_2_1 75
#define ZYNQMP_IRQ_TTC_2_2 76
#define ZYNQMP_IRQ_TTC_3_0 77
#define ZYNQMP_IRQ_TTC_3_1 78
#define ZYNQMP_IRQ_TTC_3_2 79

#endif /* LIBBSP_SHARED_PERIPHERAL_MAPS_ZYNQMP */
