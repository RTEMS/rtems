/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup raspberrypi_4_i2c
 *
 * @brief Raspberry Pi specific I2C definitions.
 */

/*
 * Copyright (C) 2025 Shaunak Datar
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

#ifndef LIBBSP_AARCH64_RASPBERRYPI_I2C_H
#define LIBBSP_AARCH64_RASPBERRYPI_I2C_H

#include <bsp/raspberrypi.h>
#include <bsp/rpi-gpio.h>
#include <bsp/utility.h>
#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2C controller instances on Raspberry Pi 4.
 */
typedef enum {
  /**
   * @brief BSC Master 0 (GPIO 0/1)
   */
  raspberrypi_bscm0,

  /**
   * @brief BSC Master 1 (GPIO 2/3)
   */
  raspberrypi_bscm1,

  /**
   * @brief BSC Master 3 (GPIO 4/5)
   */
  raspberrypi_bscm3,

  /**
   * @brief BSC Master 4 (GPIO 6/7)
   */
  raspberrypi_bscm4,

  /**
   * @brief BSC Master 5 (GPIO 10/11)
   */
  raspberrypi_bscm5,

  /**
   * @brief BSC Master 6 (GPIO 22/23)
   */
  raspberrypi_bscm6
} raspberrypi_bsc_masters;

/**
 * @brief Initialize the I2C bus for a specified master.
 *
 * @param device The BSC master to initialize.
 * @param bus_clock The desired bus clock frequency in Hz.
 *
 * @return RTEMS status code indicating success or failure.
 */
rtems_status_code rpi_i2c_init(
  raspberrypi_bsc_masters device,
  uint32_t                bus_clock
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_AARCH64_RASPBERRYPI_I2C_H */