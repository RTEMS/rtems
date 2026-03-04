/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ESP32
 *
 * @brief Interrupt and address definitions specific to C3.
 */

/*
 * Copyright (C) 2026 Kinsey Moore <wkmoore@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LIBBSP_ESP32_C3_CHIP_DEFINITIONS_H
#define LIBBSP_ESP32_C3_CHIP_DEFINITIONS_H

#ifndef ASM

#include <bsp.h>

#endif /* ASM */

/*
 * 1-31 for the internal VIC with IRQ 0 being invalid since it corresponds to
 * machine exception. This controller is fronted by an interrupt matrix
 * supporting 62 interrupts. All cpu-direct interrupts are exposed as mapped
 * interrupts. The count of 63 below includes 62 peripheral interrupts plus
 * invalid interrupt 0.
 */
#define RISCV_MAXIMUM_EXTERNAL_INTERRUPTS 63
#define BSP_INTERRUPT_VECTOR_COUNT        RISCV_MAXIMUM_EXTERNAL_INTERRUPTS

#define BSP_INTERRUPT_CUSTOM_VALID_VECTOR

#define UHCI0_INTR            15
#define GPIO_PROCPU_INTR      16
#define GPSPI2_INTR_2         19
#define I2S_INTR              20
#define UART_INTR             21
#define UART1_INTR            22
#define LEDC_INTR             23
#define EFUSE_INTR            24
#define TWAI_INTR             25
#define USB_SERIAL_JTAG_INTR  26
#define RTC_CNTL_INTR         27
#define RMT_INTR              28
#define I2C_EXT0_INTR         29
#define TG_T0_INTR            32
#define TG_WDT_INTR           33
#define TG1_T0_INTR           34
#define TG1_WDT_INTR          35
#define SYSTIMER_TARGET0_INTR 37
#define SYSTIMER_TARGET1_INTR 38
#define SYSTIMER_TARGET2_INTR 39
#define VDIGTAL_ADC_INTR      43
#define GDMA_CH0_INTR         44
#define GDMA_CH1_INTR         45
#define GDMA_CH2_INTR         46
#define RSA_INTR              47
#define AES_INTR              48
#define SHA_INTR              49
/*
 * These interrupts aren't used on ESP32-C-series chips as they are
 * single-core, but they're useful for generating software interrupts.
 */
#define SW_INTR_0 50
#define SW_INTR_1 51
#define SW_INTR_2 52
#define SW_INTR_3 53

#define ASSIST_DEBUG_INTR      54
#define PMS_DMA_VIO_INTR       55
#define PMS_IBUS_VIO_INTR      56
#define PMS_DBUS_VIO_INTR      57
#define PMS_PERI_VIO_INTR      58
#define PMS_PERI_VIO_SIZE_INTR 59

#define RISCV_INTERRUPT_VECTOR_SOFTWARE SW_INTR_0
#define RISCV_INTERRUPT_VECTOR_TIMER    SYSTIMER_TARGET0_INTR

#define SYSTIMER_BASE        ( (uintptr_t) 0x60023000U )
#define USB_SERIAL_JTAG_BASE ( (uintptr_t) 0x60043000U )
#define RTC_CNTL_BASE        ( (uintptr_t) 0x60008000U )
#define TIMG_BASE            ( (uintptr_t) 0x6001F000U )
#define INT_MATRIX_BASE      ( (uintptr_t) 0x600c2000U )
#define SYSREG_BASE          ( (uintptr_t) 0x600c0000U )

/* ESP32-C3 ROM function addresses */
#define UART_TX_ONE_CHAR_ADDR ( (void *) (uintptr_t) 0x40000068 )
#define UART_RX_ONE_CHAR_ADDR ( (void *) (uintptr_t) 0x40000070 )
#define UART_TX_FLUSH_ADDR    ( (void *) (uintptr_t) 0x40000080 )
#define GPIO_OUTPUT_SET_ADDR  ( (void *) (uintptr_t) 0x400005b0 )

#endif /* LIBBSP_ESP32_C3_CHIP_DEFINITIONS_H */
