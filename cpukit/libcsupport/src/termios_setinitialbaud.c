/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <sys/termios.h>
#include <rtems/termiostypes.h>

int  rtems_termios_set_initial_baud(
  struct rtems_termios_tty *ttyp,
  int                       baud
)
{
  int cflags_baud;

  cflags_baud = termios_number_to_baud(baud);
  if ( cflags_baud == -1 )
    return -1;

  ttyp->termios.c_cflag = (ttyp->termios.c_cflag & ~CBAUD) | cflags_baud;
  
  return 0;
}
