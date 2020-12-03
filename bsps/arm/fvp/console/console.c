/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp/semihosting.h>

#include <rtems/console.h>
#include <rtems/termiostypes.h>

static rtems_termios_device_context arm_fvp_console_instance;

static void arm_fvp_console_write(
  rtems_termios_device_context *ctx,
  const char                   *buf,
  size_t                        len
)
{
  size_t i;

  (void) ctx;

  for ( i = 0; i < len; ++i ) {
    arm_fvp_console_output( buf[ i ] );
  }
}

static int arm_fvp_console_read( rtems_termios_device_context *ctx )
{
  (void) ctx;

  return arm_fvp_console_input();
}

static const rtems_termios_device_handler arm_fvp_console_handler = {
  .write = arm_fvp_console_write,
  .poll_read = arm_fvp_console_read,
  .mode = TERMIOS_POLLED
};

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_termios_initialize();

  (void) rtems_termios_device_install(
    CONSOLE_DEVICE_NAME,
    &arm_fvp_console_handler,
    NULL,
    &arm_fvp_console_instance
  );

  return RTEMS_SUCCESSFUL;
}
