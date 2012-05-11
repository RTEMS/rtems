/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <termios.h>
#include <rtems/termiostypes.h>
#include <rtems/dumpbuf.h>

/* forward declarations to avoid warnings */
void ppp_test_driver_set_rx(const char *expected, size_t len);
void pppasyncattach(void);
int  pppopen(struct rtems_termios_tty *tty);
int  pppclose(struct rtems_termios_tty *tty);
int  pppread(struct rtems_termios_tty *tty, rtems_libio_rw_args_t *rw_args);
int  pppwrite(struct rtems_termios_tty *tty, rtems_libio_rw_args_t *rw_args);
int  pppioctl(struct rtems_termios_tty *tty, rtems_libio_ioctl_args_t *args);
int  pppinput(int c, struct rtems_termios_tty *tty);
int  pppstart(struct rtems_termios_tty *tp);

/*
 * Define the PPP line discipline.
 */

static struct rtems_termios_linesw pppdisc = {
  pppopen, pppclose, pppread, pppwrite,
  pppinput, pppstart, pppioctl, NULL
};

const char *RXExpected;
size_t      RXLength;

void ppp_test_driver_set_rx( const char *expected, size_t len )
{
  RXExpected = expected;
  RXLength   = len;
}

void pppasyncattach(void)
{
  rtems_termios_linesw[PPPDISC] = pppdisc;
}

int  pppopen(struct rtems_termios_tty *tty)
{
  puts( "pppopen called" );
  return 0;
}

int  pppclose(struct rtems_termios_tty *tty)
{
  puts( "pppclose called" );
  return 0;
}

int  pppread(struct rtems_termios_tty *tty, rtems_libio_rw_args_t *rw_args)
{
  puts( "pppread called" );
  
  rtems_termios_enqueue_raw_characters( tty, (char *)RXExpected, RXLength );
  
  RXExpected = NULL;
  RXLength = 0;
  return 0;
}

int  pppwrite(struct rtems_termios_tty *tty, rtems_libio_rw_args_t *rw_args)
{
  int                           maximum    = rw_args->count;
  char                         *out_buffer = rw_args->buffer; 

  printf( "pppwrite called - %d bytes\n", maximum );
  rtems_print_buffer( (unsigned char *) out_buffer, maximum );
  rw_args->bytes_moved = maximum;
  rtems_termios_dequeue_characters( tty, 1 );
  return 0;
}

int  pppioctl(struct rtems_termios_tty *tty, rtems_libio_ioctl_args_t *args)
{
  puts( "pppioctl called" );
  return 0;
}

int  pppinput(int c, struct rtems_termios_tty *tty)
{
  printf( "pppinput called - with (%c)\n", c );
  return 0;
}

int  pppstart(struct rtems_termios_tty *tp)
{
  puts( "pppstart called" );
  return 0;
}


