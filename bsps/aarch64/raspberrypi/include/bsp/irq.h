/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup raspberrypi_interrupt
 *
 * @brief Interrupt definitions.
 */

/**
 * Copyright (c) 2013 Alan Cudmore
 * Copyright (c) 2022 Mohd Noor Aman
 * Copyright (c) 2024 Ning Yang
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

#ifndef LIBBSP_ARM_RASPBERRYPI_IRQ_H
#define LIBBSP_ARM_RASPBERRYPI_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <dev/irq/arm-gic-irq.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup raspberrypi_interrupt Interrupt Support
 *
 * @ingroup RTEMSBSPsARMRaspberryPi
 *
 * @brief Interrupt support.
 */

#define BCM2835_INTC_TOTAL_IRQ       216

#define BCM2835_IRQ_SET1_MIN         0
#define BCM2835_IRQ_SET2_MIN         32

#define BCM2711_IRQ_VC_PERIPHERAL_BASE 96

/* Interrupt Vectors: System Timer */
#define BCM2835_IRQ_ID_GPU_TIMER_M0    (BCM2711_IRQ_VC_PERIPHERAL_BASE + 0)
#define BCM2835_IRQ_ID_GPU_TIMER_M1    (BCM2711_IRQ_VC_PERIPHERAL_BASE + 1)
#define BCM2835_IRQ_ID_GPU_TIMER_M2    (BCM2711_IRQ_VC_PERIPHERAL_BASE + 2)
#define BCM2835_IRQ_ID_GPU_TIMER_M3    (BCM2711_IRQ_VC_PERIPHERAL_BASE + 3)

/* Interrupt Vectors: SPI */
#define BCM2711_IRQ_SPI         (BCM2711_IRQ_VC_PERIPHERAL_BASE + 54)

/* Interrupt Vectors: I2C */
#define BCM2711_IRQ_I2C ( BCM2711_IRQ_VC_PERIPHERAL_BASE + 53 )

/* Interrupt Vectors: Videocore */
#define BCM2711_IRQ_VC_PERIPHERAL_BASE 96
#define BCM2711_IRQ_AUX                (BCM2711_IRQ_VC_PERIPHERAL_BASE + 29)
#define BCM2711_IRQ_PL011_UART         (BCM2711_IRQ_VC_PERIPHERAL_BASE + 57)

#define BCM2835_IRQ_ID_USB           9
#define BCM2835_IRQ_ID_AUX           29
#define BCM2835_IRQ_ID_SPI_SLAVE     43
#define BCM2835_IRQ_ID_PWA0          45
#define BCM2835_IRQ_ID_PWA1          46
#define BCM2835_IRQ_ID_SMI           48
#define BCM2835_IRQ_ID_GPIO_0        49
#define BCM2835_IRQ_ID_GPIO_1        50
#define BCM2835_IRQ_ID_GPIO_2        51
#define BCM2835_IRQ_ID_GPIO_3        52
#define BCM2835_IRQ_ID_I2C           53
#define BCM2835_IRQ_ID_SPI           54
#define BCM2835_IRQ_ID_PCM           55
#define BCM2835_IRQ_ID_UART          57
#define BCM2835_IRQ_ID_SD            62

#define BCM2835_IRQ_ID_BASIC_BASE_ID 64
#define BCM2835_IRQ_ID_TIMER_0       64
#define BCM2835_IRQ_ID_MAILBOX_0     65
#define BCM2835_IRQ_ID_DOORBELL_0    66
#define BCM2835_IRQ_ID_DOORBELL_1    67
#define BCM2835_IRQ_ID_GPU0_HALTED   68
#define BCM2835_IRQ_ID_GPU1_HALTED   69
#define BCM2835_IRQ_ID_ILL_ACCESS_1  70
#define BCM2835_IRQ_ID_ILL_ACCESS_0  71
#define BSP_TIMER_VIRT_PPI 27
#define BSP_TIMER_PHYS_NS_PPI 30
#define BSP_VPL011_SPI 32

#define BSP_INTERRUPT_VECTOR_COUNT    BCM2835_INTC_TOTAL_IRQ
#define BSP_INTERRUPT_VECTOR_INVALID (UINT32_MAX)

#define BSP_IRQ_COUNT               (BCM2835_INTC_TOTAL_IRQ)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */
#endif /* LIBBSP_ARM_RASPBERRYPI_IRQ_H */
