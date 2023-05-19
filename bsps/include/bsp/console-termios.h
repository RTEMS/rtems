/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#ifndef BSP_CONSOLE_TERMIOS_H
#define BSP_CONSOLE_TERMIOS_H

#include <rtems/termiostypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ConsoleTermios Termios Console Driver
 *
 * @ingroup RTEMSBSPsSharedConsole
 *
 * @brief Console driver for Termios devices.
 *
 * In order to use this driver add the following lines to the Makefile.am of
 * the BSP:
 *
 * @code
 * libbsp_a_SOURCES += ../../shared/console-termios-init.c
 * libbsp_a_SOURCES += ../../shared/console-termios.c
 * libbsp_a_SOURCES += console/console-config.c
 * @endcode
 *
 * Define the console_device_table and console_device_count in the
 * console-config.c file of the BSP.
 *
 * @{
 */

/**
 * @brief Console device probe function type.
 *
 * @param[in] context The Termios device context.
 *
 * @retval true Install this device.
 * @retval false Otherwise.
 */
typedef bool (*console_device_probe)(rtems_termios_device_context *context);

/**
 * @brief Console device information.
 */
typedef struct {
  /**
   * @brief The device file path.
   *
   * The "/dev/console" device will be automatically installed as the first
   * device of console_device_table with a successful probe.
   */
  const char *device_file;

  /**
   * @brief The device probe function.
   */
  console_device_probe probe;

  /**
   * @brief The Termios device handler.
   */
  const rtems_termios_device_handler *handler;

  /**
   * @brief The Termios device flow control handler.
   */
  const rtems_termios_device_flow *flow;

  /**
   * @brief The Termios device context.
   */
  rtems_termios_device_context *context;
} console_device;

/**
 * @brief Returns true and does nothing else.
 */
bool console_device_probe_default(rtems_termios_device_context *context);

/**
 * @brief Table for console devices installed via console_initialize() during
 * system initialization.
 *
 * It must be provided by the BSP.
 *
 * @see console_device_count.
 */
extern const console_device console_device_table[];

/**
 * @brief Count of entries in the console_device_table.
 *
 * It must be provided by the BSP.
 */
extern const size_t console_device_count;

/** @{ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP_CONSOLE_TERMIOS_H */
