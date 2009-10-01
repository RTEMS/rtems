/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Dummy interrupt definitions.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_DUMMY_IRQ_H
#define LIBBSP_ARM_DUMMY_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

#define DUMMY_IRQ_WDT 0
#define DUMMY_IRQ_SOFTWARE 1
#define DUMMY_IRQ_ARM_CORE_0 2
#define DUMMY_IRQ_ARM_CORE_1 3
#define DUMMY_IRQ_TIMER_0 4
#define DUMMY_IRQ_TIMER_1 5
#define DUMMY_IRQ_UART_0 6
#define DUMMY_IRQ_UART_1 7
#define DUMMY_IRQ_PWM 8
#define DUMMY_IRQ_I2C_0 9
#define DUMMY_IRQ_SPI_SSP_0 10
#define DUMMY_IRQ_SSP_1 11
#define DUMMY_IRQ_PLL 12
#define DUMMY_IRQ_RTC 13
#define DUMMY_IRQ_EINT_0 14
#define DUMMY_IRQ_EINT_1 15
#define DUMMY_IRQ_EINT_2 16
#define DUMMY_IRQ_EINT_3 17
#define DUMMY_IRQ_ADC_0 18
#define DUMMY_IRQ_I2C_1 19
#define DUMMY_IRQ_BOD 20
#define DUMMY_IRQ_ETHERNET 21
#define DUMMY_IRQ_USB 22
#define DUMMY_IRQ_CAN 23
#define DUMMY_IRQ_SD_MMC 24
#define DUMMY_IRQ_DMA 25
#define DUMMY_IRQ_TIMER_2 26
#define DUMMY_IRQ_TIMER_3 27
#define DUMMY_IRQ_UART_2 28
#define DUMMY_IRQ_UART_3 29
#define DUMMY_IRQ_I2C_2 30
#define DUMMY_IRQ_I2S 31

#define DUMMY_IRQ_PRIORITY_VALUE_MIN 0U
#define DUMMY_IRQ_PRIORITY_VALUE_MAX 15U

/**
 * @brief Minimum vector number.
 */
#define BSP_INTERRUPT_VECTOR_MIN DUMMY_IRQ_WDT

/**
 * @brief Maximum vector number.
 */
#define BSP_INTERRUPT_VECTOR_MAX DUMMY_IRQ_I2S

void bsp_interrupt_dispatch( void);

#if 0
void lpc24xx_irq_set_priority( rtems_vector_number vector, unsigned priority);

unsigned lpc24xx_irq_priority( rtems_vector_number vector);
#endif

/** @} */

#endif /* ASM */

#endif /* LIBBSP_ARM_DUMMY_IRQ_H */
