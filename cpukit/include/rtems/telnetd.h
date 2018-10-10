/*
 *  Original Author: Fernando RUIZ CASAS (fernando.ruiz@ctv.es)
 *  May 2001
 *  Reworked by Till Straumann and .h overhauled by Joel Sherrill.
 *
 * Copyright (c) 2009 embedded brains GmbH and others.
 *
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TELNETD_H
#define _RTEMS_TELNETD_H

#include <rtems.h>
#include <rtems/shell.h>

#ifdef __cplusplus
extern "C" {
#endif

bool rtems_telnetd_login_check(
  const char *user,
  const char *passphrase
);

/**
 * @brief Telnet command type.
 */
typedef void (*rtems_telnetd_command)(
  char * /* device name */,
  void * /* arg */
);

/**
 * @brief Telnet configuration structure.
 */
typedef struct {
  /**
   * @brief Function invoked for each Telnet connection.
   *
   * The first parameter contains the device name.  The second parameter
   * contains the argument pointer of this configuration table.
   */
  rtems_telnetd_command command;

  /**
   * @brief Argument for command function.
   */
  void *arg;

  /**
   * @brief Task priority.
   *
   * Use 0 for the default value.
   */
  rtems_task_priority priority;

  /**
   * @brief Task stack size.
   *
   * Use 0 for the default value.
   */
  size_t stack_size;

  /**
   * @brief Login check function.
   *
   * Method used for login checks.  Use @c NULL to disable a login check.
   */
  rtems_shell_login_check_t login_check;

  /**
   * @brief This is an obsolete configuration option.
   *
   * It must be set to false, otherwise rtems_telnetd_start() will do nothing
   * and returns with a status of RTEMS_NOT_IMPLEMENTED.
   */
  bool keep_stdio;

  /**
   * @brief Maximum number of clients which can connect to the system at a
   * time.
   *
   * Use 0 for the default value.
   */
  uint16_t client_maximum;

  /**
   * @brief Server port number in host byte order.
   *
   * Use 0 for the default value.
   */
  uint16_t port;
} rtems_telnetd_config_table;

/**
 * @brief Starts the Telnet server using the provided configuration.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ADDRESS The command function in the configuration is
 *   @c NULL.
 * @retval RTEMS_RESOURCE_IN_USE The server port is already in use.
 * @retval RTEMS_NOT_IMPLEMENTED The keep stdio configuration option is true.
 * @retval RTEMS_UNSATISFIED Not enough resources to start the Telnet server or
 *   task priority in configuration is invalid.
 */
rtems_status_code rtems_telnetd_start(const rtems_telnetd_config_table *config);

/**
 * @brief Telnet configuration.
 *
 * The application must provide this configuration table.  It is used by
 * rtems_telnetd_initialize() to configure the Telnet subsystem.  Do not modify
 * the entries after the intialization since it is used internally.
 */
extern rtems_telnetd_config_table rtems_telnetd_config;

/**
 * @brief Initializes the Telnet subsystem.
 *
 * Uses the application provided @ref rtems_telnetd_config configuration table.
 */
rtems_status_code rtems_telnetd_initialize(void);

#ifdef __cplusplus
}
#endif

#endif
