/* 
 *  execv() - POSIX 1003.1b 3.1.2
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>

int execv(
  const char *file,
  char *const argv[]
)
{
  errno = ENOSYS;
  return -1;
}
