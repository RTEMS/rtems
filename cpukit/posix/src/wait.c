/* 
 *  waitpid() - POSIX 1003.1b 3.2.1
 *
 *  $Id$
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int wait(
  int   *stat_loc
)
{
  errno = ENOSYS;
  return -1;
}
