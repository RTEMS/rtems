/* 
 *  fork() - POSIX 1003.1b 3.1.1
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <errno.h>

int fork( void )
{
  errno = ENOSYS;
  return -1;
}
