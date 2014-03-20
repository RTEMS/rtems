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

#include <termios.h>
#include <stdint.h>
#include <sys/ioccom.h>

int tcflush( int fd, int queue )
{
  return ioctl( fd, RTEMS_IO_TCFLUSH, (intptr_t) queue );
}
