/*
 *  rewinddir() - POSIX 1003.1b - XXX
 *
 *  $Id$
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
  errno = ENOSYS;
  return -1;
}
