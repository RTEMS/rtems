/*
 *  rewinddir() - POSIX 1003.1b - XXX
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 */

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

  status = lseek( dirp->dd_fd, 0, SEEK_SET );
 
  if( status == -1 )
    return;

  dirp->dd_loc = 0;
}
