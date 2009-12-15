/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief LPC24XX interrupt definitions.
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

#ifndef LIBBSP_ARM_LPC24XX_IRQ_H
#define LIBBSP_ARM_LPC24XX_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

#define LPC24XX_IRQ_WDT 0
#define LPC24XX_IRQ_SOFTWARE 1
#define LPC24XX_IRQ_ARM_CORE_0 2
#define LPC24XX_IRQ_ARM_CORE_1 3
#define LPC24XX_IRQ_TIMER_0 4
#define LPC24XX_IRQ_TIMER_1 5
#define LPC24XX_IRQ_UART_0 6
#define LPC24XX_IRQ_UART_1 7
#define LPC24XX_IRQ_PWM 8
#define LPC24XX_IRQ_I2C_0 9
#define LPC24XX_IRQ_SPI_SSP_0 10
#define LPC24XX_IRQ_SSP_1 11
#define LPC24XX_IRQ_PLL 12
#define LPC24XX_IRQ_RTC 13
#define LPC24XX_IRQ_EINT_0 14
#define LPC24XX_IRQ_EINT_1 15
#define LPC24XX_IRQ_EINT_2 16
#define LPC24XX_IRQ_EINT_3 17
#define LPC24XX_IRQ_ADC_0 18
#define LPC24XX_IRQ_I2C_1 19
#define LPC24XX_IRQ_BOD 20
#define LPC24XX_IRQ_ETHERNET 21
#define LPC24XX_IRQ_USB 22
#define LPC24XX_IRQ_CAN 23
#define LPC24XX_IRQ_SD_MMC 24
#define LPC24XX_IRQ_DMA 25
#define LPC24XX_IRQ_TIMER_2 26
#define LPC24XX_IRQ_TIMER_3 27
#define LPC24XX_IRQ_UART_2 28
#define LPC24XX_IRQ_UART_3 29
#define LPC24XX_IRQ_I2C_2 30
#define LPC24XX_IRQ_I2S 31

#define LPC24XX_IRQ_PRIORITY_VALUE_MIN 0U
#define LPC24XX_IRQ_PRIORITY_VALUE_MAX 15U
#define LPC24XX_IRQ_PRIORITY_COUNT (LPC24XX_IRQ_PRIORITY_VALUE_MAX + 1U)
#define LPC24XX_IRQ_PRIORITY_HIGHEST LPC24XX_IRQ_PRIORITY_VALUE_MIN
#define LPC24XX_IRQ_PRIORITY_LOWEST LPC24XX_IRQ_PRIORITY_VALUE_MAX

/**
 * @brief Minimum vector number.
 */
#define BSP_INTERRUPT_VECTOR_MIN LPC24XX_IRQ_WDT

/**
 * @brief Maximum vector number.
 */
#define BSP_INTERRUPT_VECTOR_MAX LPC24XX_IRQ_I2S

void lpc24xx_irq_set_priority(rtems_vector_number vector, unsigned priority);

unsigned lpc24xx_irq_get_priority(rtems_vector_number vector);

/** @} */

#endif /* ASM */

#endif /* LIBBSP_ARM_LPC24XX_IRQ_H */
