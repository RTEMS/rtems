/* 
 *  execve() - POSIX 1003.1b 3.1.2
 *
 *  $Id$
 */

#include <errno.h>

int execve(
  const char *path,
  char *const argv[],
  char *const envp[],
  ...
)
{
  errno = ENOSYS;
  return -1;
}
