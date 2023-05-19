/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2014, 2016 embedded brains GmbH & Co. KG
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
