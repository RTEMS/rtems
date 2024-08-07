/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64RaspberryPi
 *
 * @brief API of the Watchdog driver for the raspberrypi4 bsp in RTEMS.
 */

/*
 * Copyright (C) 2024 Ning Yang
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

#ifndef LIBBSP_AARCH64_RASPBERRYPI_BSP_RPI_WATCHDOG_H
#define LIBBSP_AARCH64_RASPBERRYPI_BSP_RPI_WATCHDOG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note a brief example of expected usage.
 *
 * void raspberrypi_watchdog_example()
 * {
 *  raspberrypi_watchdog_init();
 *  raspberrypi_watchdog_start(15000);
 *
 *  raspberrypi_watchdog_reload();
 *  ...
 *  raspberrypi_watchdog_reload();
 *  
 *  raspberrypi_watchdog_stop();
 * }
 * 
 */

/**
 * @brief Initialize BSP watchdog routines.
 */
void raspberrypi_watchdog_init(void);

/**
 * @brief Turn on the watchdog / begin the counter at the desired value.
 *
 * @param timeout Watchdog timeout value in ms. 
 *                The watchdog device has 20 bits of timeout, so it only 
 *                supports a maximum of 15999 ms for its timeout. 
 *                This value should be between 0 and 15999.
 */
void raspberrypi_watchdog_start(uint32_t timeout_ms);

/**
 * @brief Turn off the watchdog.
 */
void raspberrypi_watchdog_stop(void);

/**
 * @brief Reload watchdog.
 */
void raspberrypi_watchdog_reload(void);

/**
 * @brief Get the remaining time of the watchdog.
 *        The return value is still valid when the watchdog has been stopped.
 *
 * @retval Watchdog remaining time in ms.
 */
uint32_t raspberrypi_watchdog_get_remaining_time(void);

#ifdef __cplusplus
}
#endif

#endif