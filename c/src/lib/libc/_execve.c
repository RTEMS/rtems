#include <rtems.h>

#if defined(RTEMS_NEWLIB)
/*
 *  RTEMS Dummy _execveImplementation
 *
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

#include <unistd.h>
#include <errno.h>

int _execve(
  const char *path,
  char *const argv[],
  char *const environ[]
)
{
  errno = ENOSYS;
  return -1;
}
#endif
