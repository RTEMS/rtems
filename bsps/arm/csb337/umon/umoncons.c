/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * uMon Support Output Driver
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified by Fernando Nicodemos <fgnicodemos@terra.com.br>
 *  from NCB - Sistemas Embarcados Ltda. (Brazil)
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

#include <rtems/umon.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>

/* static function prototypes */
static int     umoncons_first_open(int major, int minor, void *arg);
static int     umoncons_last_close(int major, int minor, void *arg);
static int     umoncons_read(int minor);
static ssize_t umoncons_write(int minor, const char *buf, size_t len);
static void    umoncons_init(int minor);
static void    umoncons_write_polled(int minor, char c);
static int     umoncons_set_attributes(int minor, const struct termios *t);

/* Pointers to functions for handling the UART. */
const console_fns umoncons_fns =
{
  libchip_serial_default_probe,
  umoncons_first_open,
  umoncons_last_close,
  umoncons_read,
  umoncons_write,
  umoncons_init,
  umoncons_write_polled,   /* not used in this driver */
  umoncons_set_attributes,
  false                    /* TRUE if interrupt driven, FALSE if not. */
};

/*********************************************************************/
/* Functions called via callbacks (i.e. the ones in uart_fns */
/*********************************************************************/

/*
 * This is called the first time each device is opened. Since
 * the driver is polled, we don't have to do anything. If the driver
 * were interrupt driven, we'd enable interrupts here.
*/
static int umoncons_first_open(int major, int minor, void *arg)
{
  return 0;
}


/*
 * This is called the last time each device is closed.  Since
 * the driver is polled, we don't have to do anything. If the driver
 * were interrupt driven, we'd disable interrupts here.
*/
static int umoncons_last_close(int major, int minor, void *arg)
{
  return 0;
}


/*
 * Read one character from UART.
 *
 * return -1 if there's no data, otherwise return
 * the character in lowest 8 bits of returned int.
*/
static int umoncons_read(int minor)
{
  if ( !mon_gotachar() )
    return -1;
  return mon_getchar();
}


/*
 * Write buffer to LCD
 *
 * return 1 on success, -1 on error
*/
static ssize_t umoncons_write(int minor, const char *buf, size_t len)
{
  size_t i;

  for ( i=0 ; i<len ; i++ )
    mon_putchar( buf[i] );

  return len;
}


/* Set up the uMon driver. */
static void umoncons_init(int minor)
{
  rtems_umon_connect();
}

/* This is used for putchark support */
static void umoncons_write_polled(int minor, char c)
{
  mon_putchar( c );
}

/* This is for setting baud rate, bits, etc. */
static int umoncons_set_attributes(int minor, const struct termios *t)
{
  return 0;
}

/***********************************************************************/
/*
 * The following functions are not used by TERMIOS, but other RTEMS
 * functions use them instead.
 */
/***********************************************************************/
/*
 * Read from UART. This is used in the exit code, and can't
 * rely on interrupts.
*/
int umoncons_poll_read(int minor)
{
  if (!mon_gotachar())
    return -1;
  return mon_getchar();
}
