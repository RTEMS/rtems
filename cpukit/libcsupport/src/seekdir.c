/*
 *  seekdir() - POSIX 1003.1b - XXX
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

#ifndef HAVE_SEEKDIR

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

  if ( !dirp )
    return;

  status = lseek( dirp->dd_fd, loc, SEEK_SET );

  /*
   * This is not a nice way to error out, but we have no choice here.
   */

  if ( status == -1 )
    return;

  dirp->dd_loc = 0;
}

#endif
