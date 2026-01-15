/**
 * @file
 * @ingroup RTEMSBSPsShared
 * @brief Xilinx Versal Peripheral memory map.
 */

/*
 * Copyright (C) 2025 Airbus Defence and Space
 *
 * Written by Remi Debord <remi.debord@airbus.com>
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

#ifndef LIBBSP_SHARED_PERIPHERAL_MAPS_VERSAL
#define LIBBSP_SHARED_PERIPHERAL_MAPS_VERSAL

/* Data derived from https://docs.amd.com/r/en-US/am011-versal-acap-trm/4-GB-Processor-System-Address-Map */

/* LPD IO Peripherals */
#define VERSAL_UART0 (0xFF000000)
#define VERSAL_UART1 (0xFF010000)
#define VERSAL_I2C0 (0xFF020000)
#define VERSAL_I2C1 (0xFF030000)
#define VERSAL_SPI0 (0xFF040000)
#define VERSAL_SPI1 (0xFF050000)
#define VERSAL_CAN0 (0xFF060000)
#define VERSAL_CAN1 (0xFF070000)
#define VERSAL_GPIO (0xFF0B0000)
#define VERSAL_GEM0 (0xFF0C0000)
#define VERSAL_GEM1 (0xFF0D0000)

/* LPD System Registers */
#define VERSAL_IPI (0xFF300000)
#define VERSAL_TTC0 (0xFF0E0000)
#define VERSAL_TTC1 (0xFF0F0000)
#define VERSAL_TTC2 (0xFF100000)
#define VERSAL_TTC3 (0xFF110000)
#define VERSAL_LPD_SWDT (0xFF120000)
#define VERSAL_OCM_XMPU (0xFF980000)
#define VERSAL_LPD_XPPU (0xFF990000)
#define VERSAL_LPD_DMA_CH0 (0xFFA80000)
#define VERSAL_LPD_DMA_CH1 (0xFFA90000)
#define VERSAL_LPD_DMA_CH2 (0xFFAA0000)
#define VERSAL_LPD_DMA_CH3 (0xFFAB0000)
#define VERSAL_LPD_DMA_CH4 (0xFFAC0000)
#define VERSAL_LPD_DMA_CH5 (0xFFAD0000)
#define VERSAL_LPD_DMA_CH6 (0xFFAE0000)
#define VERSAL_LPD_DMA_CH7 (0xFFAF0000)

/* System Interrupt Table */

/* SPIs */
#define VERSAL_IRQ_UART_0 50
#define VERSAL_IRQ_UART_1 51

#define VERSAL_IRQ_TTC_0_0 69
#define VERSAL_IRQ_TTC_0_1 70
#define VERSAL_IRQ_TTC_0_2 71
#define VERSAL_IRQ_TTC_1_0 72
#define VERSAL_IRQ_TTC_1_1 73
#define VERSAL_IRQ_TTC_1_2 74
#define VERSAL_IRQ_TTC_2_0 75
#define VERSAL_IRQ_TTC_2_1 76
#define VERSAL_IRQ_TTC_2_2 77
#define VERSAL_IRQ_TTC_3_0 78
#define VERSAL_IRQ_TTC_3_1 79
#define VERSAL_IRQ_TTC_3_2 80

#endif /* LIBBSP_SHARED_PERIPHERAL_MAPS_VERSAL */
