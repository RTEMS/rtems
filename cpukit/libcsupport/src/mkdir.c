/*
 *  mkdir() - POSIX 1003.1b 5.4.1 - Make a Directory
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

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

int mkdir(
  const char *pathname,
  mode_t      mode
)
{
  return mknod( pathname, mode | S_IFDIR, 0LL);
}
