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
   * If this parameter is equal to zero, then the priority of network task is
   * used or 100 if this priority is less than two.
   */
  rtems_task_priority priority;

  /**
   * @brief Task stack size.
   */
  size_t stack_size;

  /**
   * @brief Login check function.
   *
   * Method used for login checks.  Use @c NULL to disable a login check.
   */
  rtems_shell_login_check_t login_check;

  /**
   * @brief Keep standard IO of the caller.
   *
   * Telnet takes over the standard input, output and error associated with
   * task, if this parameter is set to @c true.  In this case, it will @b not
   * listen on any sockets.  When this parameter is @c false, Telnet will
   * create other tasks for the shell which listen on sockets.
   */
  bool keep_stdio;
} rtems_telnetd_config_table;

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
