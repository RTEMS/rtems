/* 
 *  execvp() - POSIX 1003.1b 3.1.2
 *
 *  $Id$
 */

#include <errno.h>

int execvp(
  const char *path,
  char const *argv[]
)
{
  errno = ENOSYS;
  return -1;
}
