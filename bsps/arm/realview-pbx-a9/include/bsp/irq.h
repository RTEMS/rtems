/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup realview-pbx-a9_interrupt
 *
 * @brief Interrupt definitions.
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

#ifndef LIBBSP_ARM_REALVIEW_PBX_A9_IRQ_H
#define LIBBSP_ARM_REALVIEW_PBX_A9_IRQ_H

#ifndef ASM

#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#include <bsp/arm-a9mpcore-irq.h>
#include <dev/irq/arm-gic-irq.h>

/**
 * @defgroup realview-pbx-a9_interrupt Interrrupt Support
 *
 * @ingroup RTEMSBSPsARMRealviewPBXA9
 *
 * @brief Interrupt support.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RVPBXA9_IRQ_WATCHDOG_0 32
#define RVPBXA9_IRQ_SW_IRQ 33
#define RVPBXA9_IRQ_TIMER_0_1 36
#define RVPBXA9_IRQ_TIMER_2_3 37
#define RVPBXA9_IRQ_GPIO_0 38
#define RVPBXA9_IRQ_GPIO_1 39
#define RVPBXA9_IRQ_GPIO_2 40
#define RVPBXA9_IRQ_RTC 42
#define RVPBXA9_IRQ_SSP 43
#define RVPBXA9_IRQ_UART_0 44
#define RVPBXA9_IRQ_UART_1 45
#define RVPBXA9_IRQ_UART_2 46
#define RVPBXA9_IRQ_UART_3 47
#define RVPBXA9_IRQ_SCI 48
#define RVPBXA9_IRQ_MCI_A 49
#define RVPBXA9_IRQ_MCI_B 50
#define RVPBXA9_IRQ_AACI 51
#define RVPBXA9_IRQ_KMI0 52
#define RVPBXA9_IRQ_KMI1 53
#define RVPBXA9_IRQ_CLCD 55
#define RVPBXA9_IRQ_DMAC 56
#define RVPBXA9_IRQ_PWRFAIL 57
#define RVPBXA9_IRQ_CF_INT 59
#define RVPBXA9_IRQ_ETHERNET 60
#define RVPBXA9_IRQ_USB 61
#define RVPBXA9_IRQ_T1_INT_0 64
#define RVPBXA9_IRQ_T1_INT_1 65
#define RVPBXA9_IRQ_T1_INT_2 66
#define RVPBXA9_IRQ_T1_INT_3 67
#define RVPBXA9_IRQ_T1_INT_4 68
#define RVPBXA9_IRQ_T1_INT_5 69
#define RVPBXA9_IRQ_T1_INT_6 70
#define RVPBXA9_IRQ_T1_INT_7 71
#define RVPBXA9_IRQ_WATCHDOG_1 72
#define RVPBXA9_IRQ_TIMER_4_5 73
#define RVPBXA9_IRQ_TIMER_6_7 74
#define RVPBXA9_IRQ_PCI_INTR 80
#define RVPBXA9_IRQ_P_NMI 81
#define RVPBXA9_IRQ_P_NINT_0 82
#define RVPBXA9_IRQ_P_NINT_1 83
#define RVPBXA9_IRQ_P_NINT_2 84
#define RVPBXA9_IRQ_P_NINT_3 85
#define RVPBXA9_IRQ_P_NINT_4 86
#define RVPBXA9_IRQ_P_NINT_5 87
#define RVPBXA9_IRQ_P_NINT_6 88
#define RVPBXA9_IRQ_P_NINT_7 89

#define BSP_INTERRUPT_VECTOR_COUNT 90

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_REALVIEW_PBX_A9_IRQ_H */
