/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
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

#include <bsp/console-termios.h>
#include <bsp/fatal.h>

#include <rtems/console.h>

#include <unistd.h>

bool console_device_probe_default(rtems_termios_device_context *context)
{
  (void) context;

  return true;
}

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  bool console_device_done = false;

  rtems_termios_initialize();

  for ( minor = 0; minor < console_device_count; ++minor ) {
    const console_device *ctx = &console_device_table[ minor ];
    rtems_status_code     sc;

    if ( ( *ctx->probe )( ctx->context ) ) {
      sc = rtems_termios_device_install(
        ctx->device_file,
        ctx->handler,
        ctx->flow,
        ctx->context
      );
      if ( sc != RTEMS_SUCCESSFUL ) {
        bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_0 );
      }

      if ( !console_device_done ) {
        console_device_done = true;

        if ( link( ctx->device_file, CONSOLE_DEVICE_NAME ) != 0 ) {
          bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_1 );
        }
      }
    }
  }

  return RTEMS_SUCCESSFUL;
}
