/*
 *  seekdir() - POSIX 1003.1b - XXX
 *
 *  $Id$
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
  errno = ENOSYS;
  return -1;
}
