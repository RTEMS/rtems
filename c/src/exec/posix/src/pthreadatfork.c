/* 
 *  pthread_atfork() - POSIX 1003.1b 3.1.3
 *
 *  3.1.3 Register Fork Handlers, P1003.1c/Draft 10, P1003.1c/Draft 10, p. 27
 *
 *  RTEMS does not support processes, so we fall under this and do not
 *  provide this routine:
 *
 *  "Either the implementation shall support the pthread_atfork() function
 *   as described above or the pthread_atfork() funciton shall not be
 *   provided."
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <errno.h>

int pthread_atfork(
  void (*prepare)(void),
  void (*parent)(void),
  void (*child)(void)
)
{
  errno = ENOSYS;
  return -1;
}
