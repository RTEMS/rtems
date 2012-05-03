/*
 *  LCD Console Output Driver for CSBx37
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified by Fernando Nicodemos <fgnicodemos@terra.com.br>
 *  from NCB - Sistemas Embarcados Ltda. (Brazil)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <termios.h>

#include <rtems/bspIo.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>
#include "sed1356.h"

/* static function prototypes */
static int     fbcons_first_open(int major, int minor, void *arg);
static int     fbcons_last_close(int major, int minor, void *arg);
static int     fbcons_read(int minor);
static ssize_t fbcons_write(int minor, const char *buf, size_t len);
static void    fbcons_init(int minor);
static void    fbcons_write_polled(int minor, char c);
static int     fbcons_set_attributes(int minor, const struct termios *t);

/* Pointers to functions for handling the UART. */
console_fns fbcons_fns =
{
  libchip_serial_default_probe,
  fbcons_first_open,
  fbcons_last_close,
  fbcons_read,
  fbcons_write,
  fbcons_init,
  fbcons_write_polled,   /* not used in this driver */
  fbcons_set_attributes,
  FALSE      /* TRUE if interrupt driven, FALSE if not. */
};
/*********************************************************************/
/* Functions called via callbacks (i.e. the ones in uart_fns */
/*********************************************************************/

/*
 * This is called the first time each device is opened. Since
 * the driver is polled, we don't have to do anything. If the driver
 * were interrupt driven, we'd enable interrupts here.
 */
static int fbcons_first_open(int major, int minor, void *arg)
{
  /* printk( "Frame buffer -- first open\n" ); */
  return 0;
}


/*
 * This is called the last time each device is closed.  Since
 * the driver is polled, we don't have to do anything. If the driver
 * were interrupt driven, we'd disable interrupts here.
 */
static int fbcons_last_close(int major, int minor, void *arg)
{
  /* printk( "Frame buffer -- last close\n" ); */
  return 0;
}


/*
 * Read one character from UART.
 *
 * return -1 if there's no data, otherwise return
 * the character in lowest 8 bits of returned int.
 */
static int fbcons_read(int minor)
{
  /* printk( "Frame buffer -- read\n" ); */
  return -1;
}


/*
 * Write buffer to LCD
 *
 * return 1 on success, -1 on error
 */
static ssize_t fbcons_write(int minor, const char *buf, size_t len)
{
  int i;

  /* printk( "Frame buffer -- write\n" ); */
  for ( i=0 ; i<len ; i++ )
    sed_putchar( buf[i] );

  return 1;
}


/* Set up the LCD controller. */
static void fbcons_init(int minor)
{
  /* printk( "Initializing frame buffer\n" ); */
  sed_init();
}

/* This is used for putchark support */
static void fbcons_write_polled(int minor, char c)
{
  /* printk( "frame buffer -- write polled\n" ); */
  sed_putchar( c );
}

/* This is for setting baud rate, bits, etc. */
static int fbcons_set_attributes(int minor, const struct termios *t)
{
  /* printk( "frame buffer -- set attributes\n" ); */
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
int fbcons_poll_read(int minor)
{
  /* printk( "frame buffer -- poll read\n" ); */
  return -1;
}
