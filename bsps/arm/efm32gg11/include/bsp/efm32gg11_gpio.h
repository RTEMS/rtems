/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMEFM32GG11
 *
 * @brief EFM32GG11 GPIO Support
 */

/*
 * Copyright (C) 2025 Allan N. Hessenflow
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

#ifndef LIBBSP_ARM_EFM32GG11_GPIO_H
#define LIBBSP_ARM_EFM32GG11_GPIO_H

#include <bspopts.h>

void efm32gg11_gpio_init_0(void);
void efm32gg11_gpio_init(void);

/* returns a non-negative number on success indicating the interrupt
   number assigned, or a negative number on failure. */
int efm32gg11_gpio_int_register(uint8_t port, uint8_t pin,
                                void (*cb)(void *), void *arg);

/* which is the bit number from the em4 wakeup pin registers, range 16 - 31 */
void efm32gg11_gpio_wake_pin_int_register(unsigned int which,
                                          void (*cb)(void *), void *arg);

/* which is the interrupt number provided by efm32gg11_gpio_int_register() */
void efm32gg11_gpio_int_mode(unsigned int which, bool rising, bool falling);

/* which is the bit number from the em4 wakeup pin registers, range 16 - 31 */
void efm32gg11_gpio_wake_pin_level(unsigned int which, bool high);

/* which is the interrupt number provided by efm32gg11_gpio_int_register() */
void efm32gg11_gpio_int_enable(unsigned int which, bool enable);

void efm32gg11_gpio_mode(uint8_t port, uint8_t pin, uint8_t mode);

void efm32gg11_gpio_clear_set(uint8_t port, uint16_t clear, uint16_t set);

#endif /* LIBBSP_ARM_EFM32GG11_GPIO_H */
