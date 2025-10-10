/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2012, 2015 embedded brains GmbH & Co. KG
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
 * Console driver for Lauterbach Trace32 Simulator.  The implementation is
 * based on the example in "demo/powerpc/etc/terminal/terminal_mpc85xx.cmm" in
 * the Trace32 system directory.
 */

#include <rtems/bspIo.h>
#include <rtems/console.h>
#include <rtems/termiostypes.h>

RTEMS_SECTION(".rtemsrwset.t32") volatile unsigned char messagebufferin[256];

RTEMS_SECTION(".rtemsrwset.t32") volatile unsigned char messagebufferout[256];

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
  (void) base;
  (void) term;
  (void) args;

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
  (void) base;

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
  (void) major;
  (void) minor;
  (void) arg;

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
