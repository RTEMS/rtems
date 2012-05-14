/*
 *  waitpid() - POSIX 1003.1 3.2.1
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <rtems/seterr.h>

int waitpid(
  pid_t pid,
  int *stat_loc,
  int options
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
