/*
 *  tcflow() - POSIX 1003.1b 7.2.2 - Line Control Functions
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

#if defined(RTEMS_NEWLIB)
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <rtems/seterr.h>

int tcflow (
  int fd __attribute__((unused)),
  int action
)
{
  switch (action) {
    case TCOOFF:
    case TCOON:
    case TCIOFF:
    case TCION:
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* fd is not validated */

  /* When this is supported, implement it here */
  rtems_set_errno_and_return_minus_one( ENOTSUP );
}

#endif
