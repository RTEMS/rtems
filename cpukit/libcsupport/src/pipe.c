/*
 *  pipe() - POSIX 1003.1b 6.1.1 Create an Inter-Process Channel
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

#include <unistd.h>

#include <errno.h>
#include <sys/types.h>
#include <rtems/seterr.h>
#include <rtems/pipe.h>

int pipe(
  int filsdes[2]
)
{
  if (filsdes == NULL)
    rtems_set_errno_and_return_minus_one( EFAULT );

  return pipe_create(filsdes);
}
