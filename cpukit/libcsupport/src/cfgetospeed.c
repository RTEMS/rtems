/*
 *  cfgetospeed() - POSIX 1003.1b 7.1.3 - Baud Rate Functions
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

speed_t cfgetospeed(
  const struct termios *tp
)
{
  return tp->c_cflag & CBAUD;
}
#endif
