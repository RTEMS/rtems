/* 
 *  pthread_atfork() - POSIX 1003.1b 3.1.3
 *
 *  $Id$
 */

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
