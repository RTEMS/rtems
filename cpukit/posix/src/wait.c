/* 
 *  waitpid() - POSIX 1003.1b 3.2.1
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

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
