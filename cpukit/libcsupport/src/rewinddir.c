/*
 *  rewinddir() - POSIX 1003.1b - XXX
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

void rewinddir(
  DIR *dirp
)
{
  off_t status;

  if ( !dirp )
    return;

  status = lseek( dirp->dd_fd, 0, SEEK_SET );

  if( status == -1 )
    return;

  dirp->dd_loc = 0;
}
