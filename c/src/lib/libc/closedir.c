/*
 *  closedir() - POSIX 1003.1b - XXX
 *
 *  $Id$
 */


#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int closedir(
  register DIR *dirp
)
{
  errno = ENOSYS;
  return -1;
}
