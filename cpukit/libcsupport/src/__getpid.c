/*
 *  Some C Libraries reference this routine since they think getpid is
 *  a real system call.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

pid_t __getpid(void)
{
  return getpid();
}
