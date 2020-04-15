/**
 *  @file
 *
 *  @brief Line Control Functions
 *  @ingroup Termios
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB)
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <rtems/seterr.h>

int tcflow (
  int fd,
  int action
)
{
  (void) fd;

  switch (action) {
    case TCOOFF:
    case TCOON:
    case TCIOFF:
    case TCION:
      break;
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* fd is not validated */

  /* When this is supported, implement it here */
  rtems_set_errno_and_return_minus_one( ENOTSUP );
}

#endif
