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

#include <bsp.h>
#include <bsp/console-polled.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/* Most of the code below is copied from riscv-pk project */
# define TOHOST_CMD(dev, cmd, payload) \
  (((uint64_t)(dev) << 56) | ((uint64_t)(cmd) << 48) | (uint64_t)(payload))

#define FROMHOST_DEV(fromhost_value) ((uint64_t)(fromhost_value) >> 56)
#define FROMHOST_CMD(fromhost_value) ((uint64_t)(fromhost_value) << 8 >> 56)
#define FROMHOST_DATA(fromhost_value) ((uint64_t)(fromhost_value) << 16 >> 16)

volatile uint64_t tohost __attribute__((section(".htif")));
volatile uint64_t fromhost __attribute__((section(".htif")));
volatile int htif_console_buf;

static void __check_fromhost()
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

int htif_console_getchar()
{
  __check_fromhost();
  int ch = htif_console_buf;
  if (ch >= 0) {
    htif_console_buf = -1;
    __set_tohost(1, 0, 0);
  }

  return ch - 1;
}

static void do_tohost_fromhost(uintptr_t dev, uintptr_t cmd, uintptr_t data)
{
  __set_tohost(dev, cmd, data);

  while (1) {
    uint64_t fh = fromhost;
    if (fh) {
      if (FROMHOST_DEV(fh) == dev && FROMHOST_CMD(fh) == cmd) {
        fromhost = 0;
        break;
      }
      __check_fromhost();
    }
  }
}

void htif_console_putchar(uint8_t ch)
{
  __set_tohost(1, 1, ch);
}

void htif_poweroff()
{
  while (1) {
    fromhost = 0;
    tohost = 1;
  }
}

void console_initialize_hardware(void)
{
  /* Do nothing */
}

static void outbyte_console(char ch)
{
  htif_console_putchar(ch);
}

static char inbyte_console(void)
{
  return htif_console_getchar();
}

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */
void console_outbyte_polled(
  int  port,
  char ch
)
{
  outbyte_console( ch );
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(int port)
{
  char c;

  c = inbyte_console();
  if (!c) {
    return -1;
  }
  return (int) c;
}

/*
 *  To support printk
 */

#include <rtems/bspIo.h>

static void RISCV_output_char(char c)
{
  console_outbyte_polled( 0, c );
}

BSP_output_char_function_type BSP_output_char = RISCV_output_char;
BSP_polling_getchar_function_type BSP_poll_char =
  (void *)console_inbyte_nonblocking;
