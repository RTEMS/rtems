/*
 * Copyright (c) 2012, 2015 embedded brains GmbH.  All rights reserved.
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

/*
 * Console driver for Lauterbach Trace32 Simulator.  The implementation is
 * based on the example in "demo/powerpc/etc/terminal/terminal_mpc85xx.cmm" in
 * the Trace32 system directory.
 */

#include <rtems/bspIo.h>
#include <rtems/console.h>
#include <rtems/termiostypes.h>

volatile unsigned char messagebufferin[256];

volatile unsigned char messagebufferout[256];

typedef struct {
  rtems_termios_device_context base;
  int input_size;
  int input_index;
} t32_console_context;

static t32_console_context t32_console_instance;

static bool t32_console_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  rtems_termios_set_initial_baud(tty, 115200);

  return true;
}

static int t32_console_read_polled(rtems_termios_device_context *base)
{
  t32_console_context *ctx = (t32_console_context *) base;
  int c;

  if (ctx->input_size == 0) {
    int new_bufsize = messagebufferin[0];

    if (new_bufsize != 0) {
      ctx->input_size = new_bufsize;
      ctx->input_index = 0;
    } else {
      return -1;
    }
  }

  c = messagebufferin[4 + ctx->input_index];

  ++ctx->input_index;
  if (ctx->input_index >= ctx->input_size) {
    messagebufferin[0] = 0;
    ctx->input_size = 0;
  }

  return c;
}

static void t32_console_write_char_polled(char c)
{
  while (messagebufferout[0] != 0) {
    /* Wait for ready */
  }

  messagebufferout[4] = (unsigned char) c;
  messagebufferout[0] = 1;
}

static void t32_console_write_polled(
  rtems_termios_device_context *base,
  const char *s,
  size_t n
)
{
  size_t i;

  for (i = 0; i < n; ++i) {
    t32_console_write_char_polled(s[i]);
  }
}

const rtems_termios_device_handler t32_console_handler = {
  .first_open = t32_console_first_open,
  .poll_read = t32_console_read_polled,
  .write = t32_console_write_polled,
  .mode = TERMIOS_POLLED
};

rtems_device_driver console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  t32_console_context *ctx = &t32_console_instance;

  rtems_termios_initialize();
  rtems_termios_device_context_initialize(&ctx->base, "T32 Console");
  rtems_termios_device_install(
    CONSOLE_DEVICE_NAME,
    &t32_console_handler,
    NULL,
    &ctx->base
  );

  return RTEMS_SUCCESSFUL;
}

BSP_output_char_function_type BSP_output_char = t32_console_write_char_polled;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
