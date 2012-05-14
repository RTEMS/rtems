/*
 *  tcsetattr() - POSIX 1003.1b 7.2.1 - Get and Set State
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#if defined(RTEMS_NEWLIB)

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <termios.h>
/* #include <sys/ioctl.h> */

#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>

int tcsetattr(
  int             fd,
  int             opt,
  struct termios *tp
)
{
  switch (opt) {
  default:
    rtems_set_errno_and_return_minus_one( ENOTSUP );

  case TCSADRAIN:
    if (ioctl( fd, RTEMS_IO_TCDRAIN, NULL ) < 0)
    	return -1;
    /*
     * Fall through to....
     */
  case TCSANOW:
    return ioctl( fd, RTEMS_IO_SET_ATTRIBUTES, tp );
  }
}
#endif
