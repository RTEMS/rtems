/*
 *  seekdir() - POSIX 1003.1b - XXX
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

#include <sys/param.h>
#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

void seekdir(
   DIR  *dirp,
   long  loc
)
{
  off_t status;

  status = lseek( dirp->dd_fd, loc, SEEK_SET );

  /*
   * This is not a nice way to error out, but we have no choice here.
   */
  if( status == -1 ) 
    return;

  dirp->dd_loc = 0;
}
