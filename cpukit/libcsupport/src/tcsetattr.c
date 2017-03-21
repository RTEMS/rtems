/**
 *  @file
 *
 *  @brief Get and Set State
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

/**
 *  POSIX 1003.1b 7.2.1 - Get and Set State
 */
int tcsetattr(
  int                   fd,
  int                   opt,
  const struct termios *tp
)
{
  struct termios localterm;

  if (opt & TCSASOFT) {
    localterm = *tp;
    localterm.c_cflag |= CIGNORE;
    tp = &localterm;
  }

  switch (opt & ~TCSASOFT) {

  case TCSANOW:
    return ioctl( fd, TIOCSETA, tp );

  case TCSADRAIN:
    return ioctl( fd, TIOCSETAW, tp );

  case TCSAFLUSH:
    return ioctl( fd, TIOCSETAF, tp );

  default:
    rtems_set_errno_and_return_minus_one( EINVAL );
  }
}
#endif
