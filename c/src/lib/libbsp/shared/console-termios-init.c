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

#include <bsp/console-termios.h>
#include <bsp/fatal.h>

#include <rtems/console.h>

static rtems_interrupt_lock console_lock =
  RTEMS_INTERRUPT_LOCK_INITIALIZER( "console" );

static rtems_device_major_number console_major = UINT32_MAX;

static rtems_device_minor_number console_minor;

rtems_status_code console_device_install(
  const char                         *device_file,
  const rtems_termios_device_handler *handler,
  const rtems_termios_device_flow    *flow,
  rtems_termios_device_context       *context
)
{
  rtems_interrupt_lock_context lock_context;
  rtems_device_minor_number    minor;

  rtems_interrupt_lock_acquire( &console_lock, &lock_context );
  minor = console_minor;
  ++console_minor;
  rtems_interrupt_lock_release( &console_lock, &lock_context );

  return rtems_termios_device_install(
    device_file,
    console_major,
    minor,
    handler,
    flow,
    context
  );
}

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
        major,
        minor,
        ctx->handler,
        ctx->flow,
        ctx->context
      );
      if ( sc != RTEMS_SUCCESSFUL ) {
        bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_0 );
      }

      if ( !console_device_done ) {
        console_device_done = true;

        sc = rtems_io_register_name( CONSOLE_DEVICE_NAME, major, minor );
        if ( sc != RTEMS_SUCCESSFUL ) {
          bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_1 );
        }
      }
    }
  }

  console_major = major;
  console_minor = minor;

  return RTEMS_SUCCESSFUL;
}
