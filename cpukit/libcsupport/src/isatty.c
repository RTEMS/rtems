/*
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

#ifndef HAVE_ISATTY

#include <sys/stat.h>

int isatty(
  int fd
)
{
  struct stat buf;

  if (fstat (fd, &buf) < 0)
    return 0;

  if (S_ISCHR (buf.st_mode))
    return 1;

  return 0;
}

#endif
