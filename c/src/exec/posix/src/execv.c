/* 
 *  execv() - POSIX 1003.1b 3.1.2
 *
 *  $Id$
 */

#include <errno.h>

int execv(
  const char *file,
  char *const argv[],
  ...
{
  errno = ENOSYS;
  return -1;
}
