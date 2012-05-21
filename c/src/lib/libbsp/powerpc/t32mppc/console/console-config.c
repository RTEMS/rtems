/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
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

/*
 * Console driver for Lauterbach Trace32 Simulator.  The implementation is
 * based on the example in "demo/powerpc/etc/terminal/terminal_mpc85xx.cmm" in
 * the Trace32 system directory.
 */

#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>

#include <libchip/serial.h>
#include <libchip/sersupp.h>

#define CONSOLE_COUNT 1

volatile unsigned char messagebufferin [256];

volatile unsigned char messagebufferout [256];

static void initialize(int minor)
{
  /* Nothing to do */
}

static int first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, 115200);

  return 0;
}

static int last_close(int major, int minor, void *arg)
{
  return 0;
}

static int read_polled(int minor)
{
  static int bufsize;
  static int bufindex;

  int c;

  if (bufsize == 0) {
    int new_bufsize = messagebufferin [0];

    if (new_bufsize != 0) {
      bufsize = new_bufsize;
      bufindex = 0;
    } else {
      return -1;
    }
  }

  c = messagebufferin [4 + bufindex];

  ++bufindex;
  if (bufindex >= bufsize) {
    messagebufferin [0] = 0;
    bufsize = 0;
  }

  return c;
}

static void write_polled(int minor, char c)
{
  while (messagebufferout[0] != 0) {
    /* Wait for ready */
  }

  messagebufferout [4] = (unsigned char) c;
  messagebufferout [0] = 1;
}

static ssize_t write_support_polled(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i) {
    write_polled(minor, s [i]);
  }

  return n;
}

static int set_attribues(int minor, const struct termios *term)
{
  return -1;
}

static console_fns t32mppc_console_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = first_open,
  .deviceLastClose = last_close,
  .deviceRead = read_polled,
  .deviceWrite = write_support_polled,
  .deviceInitialize = initialize,
  .deviceWritePolled = write_polled,
  .deviceSetAttributes = set_attribues,
  .deviceOutputUsesInterrupts = false
};

unsigned long Console_Configuration_Count = CONSOLE_COUNT;

console_tbl Console_Configuration_Ports [CONSOLE_COUNT] = {
  {
    .sDeviceName = "/dev/ttyS0",
    .deviceType = SERIAL_CUSTOM,
    .pDeviceFns = &t32mppc_console_fns
  }
};

static void output_char(char c)
{
  const console_fns *con = Console_Port_Tbl [Console_Port_Minor]->pDeviceFns;

  if (c == '\n') {
    con->deviceWritePolled((int) Console_Port_Minor, '\r');
  }
  con->deviceWritePolled((int) Console_Port_Minor, c);
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
