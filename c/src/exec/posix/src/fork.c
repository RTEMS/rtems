/* 
 *  fork() - POSIX 1003.1b 3.1.1
 *
 *  $Id$
 */

#include <sys/types.h>
#include <errno.h>

int fork( void )
{
  errno = ENOSYS;
  return -1;
}
