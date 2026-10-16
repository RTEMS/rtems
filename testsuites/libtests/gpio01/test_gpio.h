/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief A GPIO controller with no hardware behind it, for gpio01.
 */

/*
 * Copyright (C) 2026 Samuel Price <thesamprice@gmail.com>
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

#ifndef _TEST_GPIO_H
#define _TEST_GPIO_H

#include <dev/gpio/gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pins chosen so that every answer the generic layer can give has a pin that
 * provokes it.  A controller where all the pins are the same would exercise
 * one path eight times.
 */
#define TEST_GPIO_PIN_COUNT 40

/*
 * Deliberately more than one bitmap word wide.  A controller of 32 pins or
 * fewer cannot tell a bulk operation that spans words from one that does
 * not, and cannot exercise a caller that allocates fewer words than the
 * controller is wide at all.
 */

/** A fully capable pin, named "LED0". */
#define TEST_GPIO_PIN_FULL 0
/** A second fully capable pin, for the operations that need two. */
#define TEST_GPIO_PIN_FULL2 1
/** A third, so that a multiple-pin operation has something to act on. */
#define TEST_GPIO_PIN_FULL3 2
/** Input only: no RTEMS_GPIO_CAP_OUTPUT. */
#define TEST_GPIO_PIN_INPUT 3
/** Reserved by the board, as a flash pad would be. */
#define TEST_GPIO_PIN_RESERVED 4
/** Not brought out to a pad in this package. */
#define TEST_GPIO_PIN_NO_PAD 5
/** A strapping pin, named "BOOT": usable, but the level at reset matters. */
#define TEST_GPIO_PIN_STRAPPING 6
/** A virtual pin, as an expander behind a bus would be. */
#define TEST_GPIO_PIN_VIRTUAL 7
/** A fully capable pin in the second bitmap word. */
#define TEST_GPIO_PIN_HIGH 33

/**
 * @brief The only drive strength this controller has, in microamperes.
 *
 * Anything else is rounded to it, which is what lets the test prove that a
 * rounded value is reported back rather than silently applied.
 */
#define TEST_GPIO_DRIVE_STRENGTH 10000

/**
 * @brief Creates the controller and registers it at @a path.
 */
/**
 * @brief Returns how many times the controller's destroy handler ran.
 *
 * @return Returns the count.
 */
/**
 * @brief Makes the next pin_configure() of the controller fail.
 *
 * @param err is the errno the handler returns, once.
 */
void test_gpio_fail_next_configure(int err);

/**
 * @brief Makes the next pin_get_info() of the controller fail.
 *
 * @param err is the errno the handler returns, once.
 */
void test_gpio_fail_next_pin_get_info(int err);

int test_gpio_destroyed(void);

int test_gpio_register(const char* path);

/**
 * @brief Registers a controller that implements pin_get_info() and nothing
 *   else, so that every other operation has to answer ENOTSUP.
 */
int test_gpio_register_minimal(const char* path);

/**
 * @brief The level on the pad, bypassing the API.
 *
 * Physical rather than logical, so a test can tell an inverted pin from a
 * pin that was written the other way round.
 */
int test_gpio_raw_level(uint32_t pin);

/**
 * @brief Drives a pad from outside, as a device on the board would.
 */
void test_gpio_set_raw_level(uint32_t pin, int value);

/**
 * @brief Raises @a pin's interrupt, if one is enabled on it.
 */
void test_gpio_fire_irq(uint32_t pin);

/**
 * @brief The options the driver was last handed for @a pin's interrupt.
 *
 * @param pin is the pin to report.
 *
 * @return Returns the options given to the driver's pin_irq_enable handler.
 */
rtems_gpio_irq_flags test_gpio_irq_flags(uint32_t pin);

#ifdef __cplusplus
}
#endif

#endif /* _TEST_GPIO_H */
