/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief LPC176X interrupt definitions.
 */

/*
 * Copyright (C) 2008, 2012 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_LPC176X_IRQ_H
#define LIBBSP_ARM_LPC176X_IRQ_H

#ifndef ASM
#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>
#endif

/**
 * @addtogroup RTEMSImplClassicIntr
 *
 * @{
 */

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

#define BSP_INTERRUPT_VECTOR_COUNT 41

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
