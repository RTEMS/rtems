/*
 *  pipe() - POSIX 1003.1b 6.1.1 Create an Inter-Process Channel
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>

int pipe(
  int filsdes[2]
)
{
  errno = ENOSYS;
  return -1;
}
