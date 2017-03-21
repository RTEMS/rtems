/*
 *  tcflush() - POSIX 1003.1b 7.2.2 - Line Control Functions
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sys/fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <sys/ioccom.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>

int tcflush( int fd, int which )
{
  int com;

  switch (which) {
  case TCIFLUSH:
    com = FREAD;
    break;
  case TCOFLUSH:
    com = FWRITE;
    break;
  case TCIOFLUSH:
    com = FREAD | FWRITE;
    break;
  default:
    rtems_set_errno_and_return_minus_one( EINVAL );
  }
  return ioctl( fd, TIOCFLUSH, &com );
}
