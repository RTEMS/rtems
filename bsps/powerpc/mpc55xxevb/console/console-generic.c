/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Generic console driver implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
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

#include <sys/cdefs.h>

#include <bsp.h>
#include <bsp/console-generic.h>
#include <bsp/fatal.h>

#include <rtems/bspIo.h>
#include <rtems/console.h>

static const struct termios console_generic_termios = {
  .c_cflag = CS8 | CREAD | CLOCAL | __CONCAT(B, BSP_DEFAULT_BAUD_RATE)
};

static void console_generic_char_out(char c)
{
  int minor = (int) console_generic_minor;
  const console_generic_callbacks *cb =
    console_generic_info_table [minor].callbacks;

  (*cb->poll_write)(minor, c);
}

static int console_generic_char_in(void)
{
  int minor = (int) console_generic_minor;
  const console_generic_callbacks *cb =
    console_generic_info_table [minor].callbacks;

  return (*cb->poll_read)(minor);
}

static void console_generic_char_out_do_init(void)
{
  int minor = (int) console_generic_minor;
  const console_generic_callbacks *cb =
    console_generic_info_table [minor].callbacks;
  const struct termios *term = &console_generic_termios;

  BSP_output_char = console_generic_char_out;
  (*cb->termios_callbacks.setAttributes)(minor, term);
}

static void console_generic_char_out_init(char c)
{
  console_generic_char_out_do_init();
  console_generic_char_out(c);
}

rtems_device_driver console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  const console_generic_info *info_table = console_generic_info_table;
  rtems_device_minor_number count = console_generic_info_count;
  rtems_device_minor_number console = console_generic_minor;

  if (count <= 0) {
    bsp_fatal(MPC55XX_FATAL_CONSOLE_GENERIC_COUNT);
  }

  rtems_termios_initialize();

  for (minor = 0; minor < count; ++minor) {
    const console_generic_info *info = info_table + minor;

    sc = rtems_io_register_name(info->device_path, major, minor);
    if (sc != RTEMS_SUCCESSFUL) {
      bsp_fatal(MPC55XX_FATAL_CONSOLE_GENERIC_REGISTER);
    }
  }

  sc = rtems_io_register_name(CONSOLE_DEVICE_NAME, major, console);
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(MPC55XX_FATAL_CONSOLE_GENERIC_REGISTER_CONSOLE);
  }

  console_generic_char_out_do_init();

  return sc;
}

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_device_minor_number count = console_generic_info_count;

  if (minor < count) {
    const console_generic_info *info = &console_generic_info_table [minor];

    sc = rtems_termios_open(
      major,
      minor,
      arg,
      &info->callbacks->termios_callbacks
    );
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_close(arg);
}

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_read(arg);
}

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_write(arg);
}

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_ioctl(arg);
}

BSP_output_char_function_type BSP_output_char = console_generic_char_out_init;

BSP_polling_getchar_function_type BSP_poll_char = console_generic_char_in;
