/**
 * @file
 *
 * @brief Generic console driver implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/console-generic.h>

#include <rtems/console.h>

static const struct termios console_generic_termios = {
  .c_cflag = CS8 | CREAD | CLOCAL | B115200
};

static void console_generic_char_out(char c)
{
  int minor = (int) console_generic_minor;
  const console_generic_callbacks *cb =
    console_generic_info_table [minor].callbacks;

  if (c == '\n') {
    (*cb->poll_write)(minor, '\r');
  }

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
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  rtems_termios_initialize();

  for (minor = 0; minor < count; ++minor) {
    const console_generic_info *info = info_table + minor;

    sc = rtems_io_register_name(info->device_path, major, minor);
    if (sc != RTEMS_SUCCESSFUL) {
      rtems_fatal_error_occurred(0xdeadbeef);
    }
  }

  sc = rtems_io_register_name(CONSOLE_DEVICE_NAME, major, console);
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
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
