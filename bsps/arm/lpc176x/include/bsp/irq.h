/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief LPC176X interrupt definitions.
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC176X_IRQ_H
#define LIBBSP_ARM_LPC176X_IRQ_H

#ifndef ASM
#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>
#endif

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

#define BSP_INTERRUPT_VECTOR_MIN 0U

#define LPC176X_IRQ_WDT 0U
#define LPC176X_IRQ_TIMER_0 1U
#define LPC176X_IRQ_TIMER_1 2U
#define LPC176X_IRQ_TIMER_2 3U
#define LPC176X_IRQ_TIMER_3 4U
#define LPC176X_IRQ_UART_0 5U
#define LPC176X_IRQ_UART_1 6U
#define LPC176X_IRQ_UART_2 7U
#define LPC176X_IRQ_UART_3 8U
#define LPC176X_IRQ_PWM_1 9U
#define LPC176X_IRQ_PLL 16U
#define LPC176X_IRQ_RTC 17U
#define LPC176X_IRQ_EINT_0 18U
#define LPC176X_IRQ_EINT_1 19U
#define LPC176X_IRQ_EINT_2 20U
#define LPC176X_IRQ_EINT_3 21U
#define LPC176X_IRQ_ADC_0 22U
#define LPC176X_IRQ_BOD 23U
#define LPC176X_IRQ_USB 24U
#define LPC176X_IRQ_CAN 25U
#define LPC176X_IRQ_DMA 26U
#define LPC176X_IRQ_I2S 27U
#define LPC176X_IRQ_SD_MMC 29U
#define LPC176X_IRQ_MCPWM 30U
#define LPC176X_IRQ_QEI 31U
#define LPC176X_IRQ_PLL_ALT 32U
#define LPC176X_IRQ_USB_ACTIVITY 33U
#define LPC176X_IRQ_CAN_ACTIVITY 34U
#define LPC176X_IRQ_UART_4 35U
#define LPC176X_IRQ_GPIO 38U
#define LPC176X_IRQ_PWM 39U
#define LPC176X_IRQ_EEPROM 40U

#define BSP_INTERRUPT_VECTOR_MAX 40

#define LPC176X_IRQ_PRIORITY_VALUE_MIN 0U

#define LPC176X_IRQ_PRIORITY_VALUE_MAX 31U

#define LPC176X_IRQ_PRIORITY_COUNT ( LPC176X_IRQ_PRIORITY_VALUE_MAX + 1U )
#define LPC176X_IRQ_PRIORITY_HIGHEST LPC176X_IRQ_PRIORITY_VALUE_MIN
#define LPC176X_IRQ_PRIORITY_LOWEST LPC176X_IRQ_PRIORITY_VALUE_MAX

#ifndef ASM

/**
 * @brief Sets the priority according to the current interruption.
 *
 * @param vector   Interrupt to be attended.
 * @param priority Interrupts priority.
 */
void lpc176x_irq_set_priority(
  rtems_vector_number vector,
  unsigned            priority
);

/**
 * @brief Gets the priority number according to the current interruption.
 *
 * @param  vector Interrupts to be attended.
 * @return The priority number according to the current interruption.
 */
unsigned lpc176x_irq_get_priority( rtems_vector_number vector );

#endif /* ASM */

/** @} */

#endif /* LIBBSP_ARM_LPC176X_IRQ_H */
