/* 
 *  execle() - POSIX 1003.1b 3.1.2
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>

int execle(
  const char *path,
  char const *arg,
  ...
)
{
  errno = ENOSYS;
  return -1;
}
