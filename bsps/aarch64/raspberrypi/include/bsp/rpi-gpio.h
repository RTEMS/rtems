/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64RaspberryPi
 *
 * @brief Raspberry Pi 4B specific GPIO definitions.
 */

/*
 * Copyright (C) 2023 Utkarsh Verma
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

#ifndef LIBBSP_AARCH64_RASPBERRYPI_BSP_RPI_GPIO_H
#define LIBBSP_AARCH64_RASPBERRYPI_BSP_RPI_GPIO_H

#include <bspopts.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Raspberry Pi GPIO functions.
 */

typedef enum {
  GPIO_INPUT,
  GPIO_OUTPUT,
  GPIO_AF5,
  GPIO_AF4,
  GPIO_AF0,
  GPIO_AF1,
  GPIO_AF2,
  GPIO_AF3,
} raspberrypi_gpio_function;

typedef enum {
  GPIO_PULL_NONE,
  GPIO_PULL_UP,
  GPIO_PULL_DOWN,
} raspberrypi_gpio_pull;

/**
 * @brief Set the operation of the general-purpose I/O pins. Each of the 58
 * GPIO pins has at least two alternative functions as defined.
 *
 * @param pin The GPIO pin.
 * @param value The optional functions are GPIO_INPUT, GPIO_OUTPUT, GPIO_AF5,
 *              GPIO_AF4, GPIO_AF0, GPIO_AF1, GPIO_AF2, GPIO_AF3.
 *
 * @retval RTEMS_SUCCESSFUL GPIO function successfully configured.
 * @retval RTEMS_INVALID_NUMBER This status code indicates that a specified
 *         number was invalid.
 */
rtems_status_code raspberrypi_gpio_set_function(
  const unsigned int  pin,
  const raspberrypi_gpio_function value
);

/**
 * @brief Set a GPIO pin.
 *
 * @param pin The GPIO pin.
 *
 * @retval RTEMS_SUCCESSFUL GPIO pin set successfully.
 * @retval RTEMS_INVALID_NUMBER This status code indicates that a specified
 *         number was invalid.
 */
rtems_status_code raspberrypi_gpio_set_pin(const unsigned int pin);

/**
 * @brief Clear a GPIO pin.
 *
 * @param pin The GPIO pin.
 *
 * @retval RTEMS_SUCCESSFUL GPIO pin clear successfully.
 * @retval RTEMS_INVALID_NUMBER This status code indicates that a specified
 *         number was invalid.
 */
rtems_status_code raspberrypi_gpio_clear_pin(const unsigned int pin);

/**
 * @brief Control the actuation of the internal pull-up/down resistors.
 *
 * @param pin The GPIO pin.
 * @param value The optional value are GPIO_PULL_NONE, GPIO_PULL_UP,
 *              GPIO_PULL_DOWN.
 *
 * @retval RTEMS_SUCCESSFUL GPIO pull set successfully.
 * @retval RTEMS_INVALID_NUMBER This status code indicates that a specified
 *         number was invalid.
 */
rtems_status_code raspberrypi_gpio_set_pull(
  const unsigned int pin,
  const raspberrypi_gpio_pull value
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_AARCH64_RASPBERRYPI_BSP_RPI_GPIO_H */
