/*
 *  scandir() - POSIX 1003.1b - XXX
 *
 *  $Id$
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int scandir(
  const char      *dirname,
  struct dirent ***namelist,
  int             (*select)(struct dirent *),
  int             (*dcomp)(const void *, const void *)
)
{
  errno = ENOSYS;
  return -1;
}
