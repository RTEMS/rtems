/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
 * @ingroup TermiostypesSupport
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
 * @brief Installs a console device after console driver initialization.
 *
 * @param[in] device_file The device file path.
 * @param[in] handler The Termios device handler.  It must be persistent
 *   throughout the installed time of the device.
 * @param[in] flow The Termios device flow control handler.  The device flow
 *   control handler are optional and may be @c NULL.  If present must be
 *   persistent throughout the installed time of the device.
 * @param[in] context The Termios device context.  It must be persistent
 *   throughout the installed time of the device.
 */
rtems_status_code console_device_install(
  const char                         *device_file,
  const rtems_termios_device_handler *handler,
  const rtems_termios_device_flow    *flow,
  rtems_termios_device_context       *context
);

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
