/*
 *  Some C Libraries reference this routine since they think getpid is
 *  a real system call.
 *
 *  $Id$
 */

#include <unistd.h>

pid_t __getpid(void)
{
  return getpid();
}

