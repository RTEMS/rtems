/* 
 *  execl() - POSIX 1003.1b 3.1.2
 *
 *  $Id$
 */

#include <errno.h>

int execl(
  const char *path,
  const char *arg,
  ...
)
{
  errno = ENOSYS;
  return -1;
}
