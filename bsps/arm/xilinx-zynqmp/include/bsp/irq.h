/**
 * @file
 * @ingroup zynq_interrupt
 * @brief Interrupt definitions.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2013 embedded brains GmbH
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

#ifndef LIBBSP_ARM_XILINX_ZYNQ_IRQ_H
#define LIBBSP_ARM_XILINX_ZYNQ_IRQ_H

#ifndef ASM

#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#include <bsp/arm-a9mpcore-irq.h>
#include <bsp/arm-gic-irq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup zynq_interrupt Interrupt Support
 * @ingroup RTEMSBSPsARMZynq
 * @brief Interrupt Support
 * @{
 */

#define ZYNQ_IRQ_CPU_0 32
#define ZYNQ_IRQ_CPU_1 33
#define ZYNQ_IRQ_L2_CACHE 34
#define ZYNQ_IRQ_OCM 35
#define ZYNQ_IRQ_PMU_0 37
#define ZYNQ_IRQ_PMU_1 38
#define ZYNQ_IRQ_XADC 39
#define ZYNQ_IRQ_DVI 40
#define ZYNQ_IRQ_SWDT 41
#define ZYNQ_IRQ_TTC_0_0 42
#define ZYNQ_IRQ_TTC_1_0 43
#define ZYNQ_IRQ_TTC_2_0 44
#define ZYNQ_IRQ_DMAC_ABORT 45
#define ZYNQ_IRQ_DMAC_0 46
#define ZYNQ_IRQ_DMAC_1 47
#define ZYNQ_IRQ_DMAC_2 48
#define ZYNQ_IRQ_DMAC_3 49
#define ZYNQ_IRQ_SMC 50
#define ZYNQ_IRQ_QUAD_SPI 51
#define ZYNQ_IRQ_GPIO 52
#define ZYNQ_IRQ_USB_0 53
#define ZYNQ_IRQ_ETHERNET_0 54
#define ZYNQ_IRQ_ETHERNET_0_WAKEUP 55
#define ZYNQ_IRQ_SDIO_0 56
#define ZYNQ_IRQ_I2C_0 57
#define ZYNQ_IRQ_SPI_0 58
#define ZYNQ_IRQ_UART_0 59
#define ZYNQ_IRQ_CAN_0 60
#define ZYNQ_IRQ_FPGA_0 61
#define ZYNQ_IRQ_FPGA_1 62
#define ZYNQ_IRQ_FPGA_2 63
#define ZYNQ_IRQ_FPGA_3 64
#define ZYNQ_IRQ_FPGA_4 65
#define ZYNQ_IRQ_FPGA_5 66
#define ZYNQ_IRQ_FPGA_6 67
#define ZYNQ_IRQ_FPGA_7 68
#define ZYNQ_IRQ_TTC_0_1 69
#define ZYNQ_IRQ_TTC_1_1 70
#define ZYNQ_IRQ_TTC_2_1 71
#define ZYNQ_IRQ_DMAC_4 72
#define ZYNQ_IRQ_DMAC_5 73
#define ZYNQ_IRQ_DMAC_6 74
#define ZYNQ_IRQ_DMAC_7 75
#define ZYNQ_IRQ_USB_1 76
#define ZYNQ_IRQ_ETHERNET_1 77
#define ZYNQ_IRQ_ETHERNET_1_WAKEUP 78
#define ZYNQ_IRQ_SDIO_1 79
#define ZYNQ_IRQ_I2C_1 80
#define ZYNQ_IRQ_SPI_1 81
#define ZYNQ_IRQ_UART_1 82
#define ZYNQ_IRQ_CAN_1 83
#define ZYNQ_IRQ_FPGA_8 84
#define ZYNQ_IRQ_FPGA_9 85
#define ZYNQ_IRQ_FPGA_10 86
#define ZYNQ_IRQ_FPGA_11 87
#define ZYNQ_IRQ_FPGA_12 88
#define ZYNQ_IRQ_FPGA_13 89
#define ZYNQ_IRQ_FPGA_14 90
#define ZYNQ_IRQ_FPGA_15 91
#define ZYNQ_IRQ_PARITY 92

#define BSP_INTERRUPT_VECTOR_MIN 0
#define BSP_INTERRUPT_VECTOR_MAX 92

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_IRQ_H */
