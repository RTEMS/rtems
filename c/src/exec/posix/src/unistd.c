/* unistd.c
 *
 *  $Id$
 */

#include <unistd.h>

#include <rtems/system.h>

#ifdef NOT_IMPLEMENTED_YET

/*
 *  4.8.1 Get Configurable System Variables, P1003.1b-1993, p. 95
 */

long sysconf(
  int name
)
{
  return POSIX_NOT_IMPLEMENTED();
}

#endif
