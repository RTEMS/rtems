/*
 *  access() - POSIX 1003.1b 5.6.3 - File Accessibility
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
#include <fcntl.h>
#include <sys/stat.h>

int access(
  const char *path,
  int         amode
)
{
  struct stat statbuf;

  if ( stat(path, &statbuf) )
    return -1;

  if ( amode & R_OK ) {
    if (!( statbuf.st_mode & S_IREAD ))
      return -1;
  }

  if ( amode & W_OK ) {
    if ( !( statbuf.st_mode & S_IWRITE ) )
      return -1;
  }

  if ( amode & X_OK ) {
    if ( !( statbuf.st_mode & S_IEXEC ) )
      return -1;
  }

  return 0;
}
