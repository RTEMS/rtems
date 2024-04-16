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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_IRQ_H
#define LIBBSP_ARM_RASPBERRYPI_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <dev/irq/arm-gic-irq.h>

/**
 * @defgroup raspberrypi_interrupt Interrrupt Support
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

#endif /* ASM */
#endif /* LIBBSP_ARM_RASPBERRYPI_IRQ_H */
