/*
 *  tcflush() - POSIX 1003.1b 7.2.2 - Line Control Functions
 *
 *  COPYRIGHT (c) 1989-2010.
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

#include <rtems/seterr.h>
#include <rtems/libio.h>

int tcflush (
  int fd __attribute__((unused)),
  int queue
)
{
  switch (queue) {
    case TCIFLUSH:
    case TCOFLUSH:
    case TCIOFLUSH:
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* fd is not validated */

  /* When this is supported, implement it here */
  rtems_set_errno_and_return_minus_one( ENOTSUP );
  return 0;
}

#endif
