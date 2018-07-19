/*
 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
 *
 * Copyright (c) 2013, The Regents of the University of California (Regents).
 * All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp/riscv.h>

#if RISCV_ENABLE_HTIF_SUPPORT != 0

#include <dev/serial/htif.h>

#include <assert.h>

/* Most of the code below is copied from riscv-pk project */
# define TOHOST_CMD(dev, cmd, payload) \
  (((uint64_t)(dev) << 56) | ((uint64_t)(cmd) << 48) | (uint64_t)(payload))

#define FROMHOST_DEV(fromhost_value) ((uint64_t)(fromhost_value) >> 56)
#define FROMHOST_CMD(fromhost_value) ((uint64_t)(fromhost_value) << 8 >> 56)
#define FROMHOST_DATA(fromhost_value) ((uint64_t)(fromhost_value) << 16 >> 16)

volatile uint64_t tohost __attribute__((section(".htif")));
volatile uint64_t fromhost __attribute__((section(".htif")));
volatile uint64_t riscv_fill_up_htif_section[510] __attribute__((section(".htif")));
volatile int htif_console_buf;

static void __check_fromhost(void)
{
  uint64_t fh = fromhost;
  if (!fh) {
    return;
  }
  fromhost = 0;

  // this should be from the console
  assert(FROMHOST_DEV(fh) == 1);
  switch (FROMHOST_CMD(fh)) {
  case 0:
    htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
    break;
  case 1:
    break;
  default:
    assert(0);
  }
}

static void __set_tohost(uintptr_t dev, uintptr_t cmd, uintptr_t data)
{
  while (tohost) {
    __check_fromhost();
  }
  tohost = TOHOST_CMD(dev, cmd, data);
}

int htif_console_getchar(rtems_termios_device_context *base)
{
  __check_fromhost();
  int ch = htif_console_buf;
  if (ch >= 0) {
    htif_console_buf = -1;
    __set_tohost(1, 0, 0);
  }

  return ch - 1;
}

void htif_console_putchar(rtems_termios_device_context *base, char c)
{
  __set_tohost(1, 1, c);
}

static void htif_console_write_polled(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  size_t i;

  for (i = 0; i < len; ++i) {
    htif_console_putchar(base, buf[i]);
  }
}

void htif_poweroff(void)
{
  while (1) {
    fromhost = 0;
    tohost = 1;
  }
}

void htif_console_context_init(
  rtems_termios_device_context *base,
  int device_tree_node
)
{
  rtems_termios_device_context_initialize(base, "HTIF");
}

static bool htif_console_first_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  return true;
}

const rtems_termios_device_handler htif_console_handler = {
  .first_open = htif_console_first_open,
  .write = htif_console_write_polled,
  .poll_read = htif_console_getchar,
  .mode = TERMIOS_POLLED
};

#endif /* RISCV_ENABLE_HTIF_SUPPORT */
