/*
 *  This file contains the TTY driver for the serial ports on the LEON.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <rtems/bspIo.h>
#include <amba.h>

/* Let user override which on-chip APBUART will be debug UART
 * 0 = Default APBUART. On MP system CPU0=APBUART0, CPU1=APBUART1...
 * 1 = APBUART[0]
 * 2 = APBUART[1]
 * 3 = APBUART[2]
 * ...
 */
int syscon_uart_index __attribute__((weak)) = 0;

/*
 *  Should we use a polled or interrupt drived console?
 *
 *  NOTE: This is defined in the custom/leon.cfg file.
 */

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

void console_outbyte_polled(
  int  port,
  char ch
);

/* body is in debugputs.c */

/*
 *  apbuart_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int apbuart_inbyte_nonblocking(int port);

/* body is in debugputs.c */


/*
 *  Console Termios Support Entry Points
 *
 */

ssize_t console_write_support (int minor, const char *buf, size_t len)
{
  int nwrite = 0, port;

  if (minor == 0)
    port = syscon_uart_index;
  else
    port = minor - 1;

  while (nwrite < len) {
    console_outbyte_polled(port, *buf++);
    nwrite++;
  }
  return nwrite;
}

int console_inbyte_nonblocking(int minor)
{
  int port;

  if (minor == 0)
    port = syscon_uart_index;
  else
    port = minor - 1;

  return apbuart_inbyte_nonblocking(port);
}

/*
 *  Console Device Driver Entry Points
 *
 */
int uarts = 0;
volatile LEON3_UART_Regs_Map *LEON3_Console_Uart[LEON3_APBUARTS];

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;
  int i;
  char console_name[16];

  rtems_termios_initialize();

  /* Update syscon_uart_index to index used as /dev/console
   * Let user select System console by setting syscon_uart_index. If the
   * BSP is to provide the default UART (syscon_uart_index==0):
   *   non-MP: APBUART[0] is system console
   *   MP: LEON CPU index select UART
   */
  if (syscon_uart_index == 0) {
#if defined(RTEMS_MULTIPROCESSING)
    syscon_uart_index = LEON3_Cpu_Index;
#else
    syscon_uart_index = 0;
#endif
  } else {
    syscon_uart_index = syscon_uart_index - 1; /* User selected sys-console */
  }

  /*  Register Device Names
   *
   *  0 /dev/console   - APBUART[USER-SELECTED, DEFAULT=APBUART[0]]
   *  1 /dev/console_a - APBUART[0] (by default not present because is console)
   *  2 /dev/console_b - APBUART[1]
   *  ...
   *
   * On a MP system one should not open UARTs that other OS instances use.
   */
  if (syscon_uart_index < uarts) {
    status = rtems_io_register_name( "/dev/console", major, 0 );
    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);
  }
  strcpy(console_name,"/dev/console_a");
  for (i = 0; i < uarts; i++) {
    if (i == syscon_uart_index)
      continue; /* skip UART that is registered as /dev/console */
    console_name[13] = 'a' + i;
    status = rtems_io_register_name( console_name, major, i+1);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
  int port;

  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    console_inbyte_nonblocking,  /* pollRead */
    console_write_support,       /* write */
    NULL,                        /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    0                            /* outputUsesInterrupts */
  };

  assert(minor <= uarts);
  if (minor > uarts || minor == (syscon_uart_index + 1))
    return RTEMS_INVALID_NUMBER;

  sc = rtems_termios_open (major, minor, arg, &pollCallbacks);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  if (minor == 0)
    port = syscon_uart_index;
  else
    port = minor - 1;

  /* Initialize UART on opening */
  LEON3_Console_Uart[port]->ctrl |= LEON_REG_UART_CTRL_RE |
                                     LEON_REG_UART_CTRL_TE;
  LEON3_Console_Uart[port]->status = 0;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}

