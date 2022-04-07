/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2013 Daniel Ramirez <javamonn@gmail.com>
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

/*
 *
 * MODULE DESCRIPTION:
 * This module implements the RTEMS drivers for the PC serial ports
 * as /dev/ttyS1 for COM1 and /dev/ttyS2 as COM2. If one of the ports
 * is used as the console, this driver would fail to initialize.
 *
 * This code was based on the console driver. It is based on the
 * current termios framework. This is just a shell around the
 * termios support.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"
#include <rtems/serial_mouse.h>
#include "../termios04/termios_testdriver_intr.h"

extern const char *Mouse_Type_Short;

bool bsp_get_serial_mouse_device(
  const char **name,
  const char **type
)
{
  *name = TERMIOS_TEST_DRIVER_DEVICE_NAME;
  *type = Mouse_Type_Short;

  return true;
}
