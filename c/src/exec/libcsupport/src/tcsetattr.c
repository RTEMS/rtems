/*
 *  tcsetattr() - POSIX 1003.1b 7.2.1 - Get and Set State
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#if defined(RTEMS_NEWLIB)

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

#include <rtems/libio.h>
#include "libio_.h"

int tcsetattr(
  int             fd,
  int             opt,
  struct termios *tp
)
{
  if ( opt != TCSANOW )
    set_errno_and_return_minus_one( ENOTSUP );

  return ioctl( fd, RTEMS_IO_SET_ATTRIBUTES, tp );
}
#endif
