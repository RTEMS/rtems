/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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
int  pppstart(struct rtems_termios_tty *tp, int len);

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

int  pppstart(struct rtems_termios_tty *tp, int len)
{
  puts( "pppstart called" );
  return 0;
}


