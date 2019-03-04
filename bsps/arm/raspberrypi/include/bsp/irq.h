/**
 * @file
 *
 * @ingroup raspberrypi_interrupt
 *
 * @brief Interrupt definitions.
 */

/**
 * Copyright (c) 2013 Alan Cudmore
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#ifndef LIBBSP_ARM_RASBPERRYPI_IRQ_H
#define LIBBSP_ARM_RASPBERRYPI_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

/**
 * @defgroup raspberrypi_interrupt Interrrupt Support
 *
 * @ingroup RTEMSBSPsARMRaspberryPi
 *
 * @brief Interrupt support.
 */

#define BCM2835_INTC_TOTAL_IRQ       64 + 8


#define BCM2835_IRQ_ID_GPU_TIMER_M0  0
#define BCM2835_IRQ_ID_GPU_TIMER_M1  1
#define BCM2835_IRQ_ID_GPU_TIMER_M2  2
#define BCM2835_IRQ_ID_GPU_TIMER_M3  3

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

#define BSP_INTERRUPT_VECTOR_MIN    (0)
#define BSP_INTERRUPT_VECTOR_MAX    (BCM2835_INTC_TOTAL_IRQ - 1)

#define BSP_IRQ_COUNT               (BCM2835_INTC_TOTAL_IRQ)

#endif /* ASM */
#endif /* LIBBSP_ARM_RASPBERRYPI_IRQ_H */
